#include <iostream>
#include "talk/base/socketstream.h"
#include "talk/base/asyncsocket.h"

#include "peer.h"

Peer::Peer(const std::string &server, const unsigned short port, const std::string& id, const std::string& role, talk_base::Thread *worker_thread) {
    server_address_ = server;
    server_port_ = port;
    id_ = id;
    role_ = role;
    isOnline_ = false;
    sock_ = NULL;

    worker_thread_ = worker_thread;
}

Peer::~Peer() {
    if ( sock_ ) {
        sock_->Close();
        delete sock_;
    }
}

void Peer::OnMessage(talk_base::Message *msg) { 
    switch (msg->message_id) {
        case MSG_START:
            onStart_w();
    }
}

void Peer::Start() {
    isOnline_ = false;
    worker_thread_->Post(this, MSG_START); 
}

int Peer::SendMessage(const std::string &to, const std::vector<std::string>& msg) {
    if ( !isOnline_ ) {
        return -1;
    }

    std::string msgPayload = "<send:" + to;
    for(int i = 0; i < (int)msg.size(); i++) {
        msgPayload = msgPayload + ":" + msg[i];
    }
    msgPayload = msgPayload + ">";
    
     
    sock_->Send( msgPayload.c_str(), msgPayload.size() );

    //std::cout << "\t===>: " << msgPayload.c_str() << std::endl;
    SignalPrintString( "\t===>: " + msgPayload );

    return 0;
}

void Peer::onStart_w() {

    // Creating socket 
    talk_base::Thread* pth = talk_base::Thread::Current();
    sock_ = pth->socketserver()->CreateAsyncSocket(SOCK_STREAM);
    sock_->SignalConnectEvent.connect(this, &Peer::onConnectEvent);
    sock_->SignalReadEvent.connect(this, &Peer::onReadEvent);
    sock_->SignalCloseEvent.connect(this, &Peer::onCloseEvent); 
    
    xmlBuffer.clear();

    // Connect to server
    talk_base::SocketAddress addr(server_address_, server_port_);
    if (sock_->Connect(addr) < 0 &&  !sock_->IsBlocking() ) {
         sock_->Close();
         delete sock_;
         sock_ = NULL;

         SignalOnline(false);
    }
}

void Peer::onConnectEvent(talk_base::AsyncSocket* socket) {
    if ( sock_->GetState() == talk_base::Socket::CS_CONNECTED) {
        std::string loginMessage = "<login:" + id_ + ":" + role_ + ">";
        sock_->Send(loginMessage.c_str(), loginMessage.size() );
    }
}

void Peer::onCloseEvent(talk_base::AsyncSocket* socket, int err) {
    
    sock_->SignalConnectEvent.disconnect(this);
    sock_->SignalReadEvent.disconnect(this);
    sock_->SignalCloseEvent.disconnect(this); 
    
    sock_->Close();
    //delete sock_;         //FIXME , we can't delete the socket in the self's callback
    sock_ = NULL;

    if ( isOnline_ == true) {
        isOnline_ = false;
        SignalOffline();
    } else {
        SignalOnline(false);
    }
}

void Peer::onReadEvent(talk_base::AsyncSocket* socket) {
    unsigned char temp[2048];

    int ret = sock_->Recv(temp, sizeof(temp) - 1);
    if ( ret > 0) {
        temp[ret] = 0;

        //std::cout << "\t<===: " << temp << std::endl;
        std::string tempStr("\t<===: ");
        tempStr = tempStr + std::string( (const char *)temp );
        SignalPrintString( tempStr );

        for(int i = 0;i < ret; i++) {
            xmlBuffer.push_back( temp[i] );
            if ( temp[i] == '>') {
                processXML();        
                xmlBuffer.clear();
            }
        }
    }
}

void Peer::processXML() {
    std::vector<std::string > words;
    
    words.clear();
    std::string currentWord = "";
    for(int i = 1; i < (int)xmlBuffer.size(); i++) {
        if ( xmlBuffer[i] == ':' || xmlBuffer[i] == '>') {
            words.push_back(currentWord);
            currentWord = "";
        } else {
            currentWord = currentWord.append(1, xmlBuffer[i]);
        }
    }

    if ( words.size() > 0) {
        if ( words[0] == "login" && words.size() == 2 ) {
            if ( words[1] == "ok") {
                isOnline_ = true;
                SignalOnline(true);
            } else {
                isOnline_ = false;
                SignalOnline(false);
            }
        } if ( words[0] == "online" && words.size() == 3) {
            SignalRemoteOnline(words[1], words[2]);
        } else if ( words[0] == "offline" ) {
            SignalRemoteOffline(words[1]);
        } else if ( words[0] == "message" ) {
            std::vector<std::string > msgBody;
            for(int j = 2; j < (int)words.size(); j++)
                msgBody.push_back(words[j]);

            SignalRemoteMessage(words[1], msgBody);
        }
    }

}

