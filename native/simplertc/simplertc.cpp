#include <iostream>

#include "simplertc.h"
#include "peer.h"
#include "rtcstream.h"

enum {
    MSG_RTC_CALL,
};

SimpleRTC::SimpleRTC(const std::string& myName) {
    peer_ = NULL;

    signal_thread_ = new talk_base::Thread();
    signal_thread_->Start();

    my_name_ = myName;
}

SimpleRTC::~SimpleRTC() {
    if ( signal_thread_ ) {
        signal_thread_->Stop();
        delete signal_thread_;
    }        
    // TODO 
}

void SimpleRTC::Login(const std::string &server, 
               const unsigned short port) {
    peer_ =  new Peer(server, port, my_name_, "mixer", signal_thread_);
    peer_->SignalOnline.connect(this, &SimpleRTC::onOnLine);
    peer_->SignalOffline.connect(this, &SimpleRTC::onOffline);
    peer_->SignalRemoteOnline.connect(this, &SimpleRTC::onRemoteOnline);
    peer_->SignalRemoteOffline.connect(this, &SimpleRTC::onRemoteOffline);
    peer_->SignalRemoteMessage.connect(this, &SimpleRTC::onRemoteMessage);
    peer_->SignalPrintString.connect(this, &SimpleRTC::onPrintString);    //for log and debug
    peer_->Start();
}

void SimpleRTC::Run() {
    talk_base::Thread* main_thread = talk_base::Thread::Current();
    main_thread->Run();
}

void SimpleRTC::OnMessage(talk_base::Message *msg) { 
    switch (msg->message_id) {
        case MSG_RTC_CALL:
            //TestCall();
            break;
    }
}

void SimpleRTC::onOnLine(bool isOk) {

}

void SimpleRTC::onOffline() {
    std::cout << "[ROOM]    Can't login to server, exiting..." << std::endl;
    exit(-1);
}

void SimpleRTC::onRemoteOnline(const std::string &remote, const std::string &role) {
#if 0
    //
    //  Debug for one one test
    //
    if ( test_factory_.get() == NULL) {
        test_factory_ = webrtc::CreateMeetingFactory("test_" + remote, "web");
        test_stream_ = new RtcStream(remote, test_factory_); 
        test_stream_->SignalSessionDescription.connect(this, &SimpleRTC::OnLocalDescription);
        test_stream_->SignalIceCandidate.connect(this, &SimpleRTC::OnLocalCandidate);
        signal_thread_->PostDelayed(1000, this, MSG_RTC_CALL);
    }
#endif
}

void SimpleRTC::onRemoteOffline(const std::string &remote) {

}

void SimpleRTC::onRemoteMessage(const std::string &remote, const std::vector<std::string>& msgBody) {

}

void SimpleRTC::onPrintString(const std::string& msg) {
//    std::cout << msg << std::endl;
}

void SimpleRTC::OnLocalCandidate(RtcStream* stream, const std::string& cand) {
    std::vector<std::string> msgBody;
    msgBody.push_back( "rtc");
    msgBody.push_back( "cand");
    msgBody.push_back( cand );
    peer_->SendMessage( stream->id(), msgBody);
}

void SimpleRTC::OnLocalDescription(RtcStream* stream, const std::string& desc) {
    std::vector<std::string> msgBody;
    msgBody.push_back( "rtc");
    msgBody.push_back( "desc");
    msgBody.push_back( desc );
    peer_->SendMessage( stream->id(), msgBody);
}

/*
void SimpleRTC::TestCall() {
    std::vector<std::string> msgBody;
    msgBody.push_back( "rtc");
    msgBody.push_back( "call");
    peer_->SendMessage( test_stream_->id(), msgBody);
}
*/


