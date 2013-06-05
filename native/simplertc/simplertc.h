#ifndef _PROBER_H_
#define _PROBER_H_

#include <vector>
#include <map>
#include "talk/base/thread.h"
#include "talk/base/messagequeue.h"

class Peer;

class RtcStream;
namespace webrtc {
    class RtcFactory;
}

class SimpleRTC : public sigslot::has_slots<>, public talk_base::MessageHandler {  
public:
    SimpleRTC(const std::string& myName);
    ~SimpleRTC();
    
    virtual void OnMessage(talk_base::Message *msg);
    void Login(const std::string &server, 
               const unsigned short port);
    
    void Run();

protected:
    // call back from peer
    void onOnLine(bool isOk);
    void onOffline();
    void onRemoteOnline(const std::string& name, const std::string& role);
    void onRemoteOffline(const std::string&);
    void onRemoteMessage(const std::string&, const std::vector<std::string>& );
    void onPrintString(const std::string& );

private:
    void OnLocalDescription(RtcStream* stream, const std::string& desc);
    void OnLocalCandidate(RtcStream* stream, const std::string& cand);

private:
    // thread resource
    talk_base::Thread *signal_thread_;

    // info resource
    std::string my_name_;
    Peer *peer_;

    RtcStream* stream_;
    talk_base::scoped_refptr<webrtc::RtcFactory> factory_;
};

#endif
