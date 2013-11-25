#ifndef _RTCSTREAM_H_
#define _RTCSTREAM_H_

#include <string>
#include "talk/base/scoped_ptr.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "rtcfactory.h"

class Peer;
class SimpleVideoRenderer;

class RtcStream : public webrtc::PeerConnectionObserver, 
                 public sigslot::has_slots<> {
public:
    RtcStream(const std::string& id, webrtc::PeerConnectionFactoryInterface* factory);            
    virtual ~RtcStream();
    
    inline std::string id() { return id_; }
    virtual void SetRenderer(SimpleVideoRenderer* r) {
        renderer_ = r;
    }
    virtual void CreateOfferDescription();
    virtual void CreateAnswerDescription();
    virtual void SetRemoteCandidate(const std::string& msg);
    virtual void SetRemoteDescription(const std::string& msg);
    virtual void SetupLocalStream(bool enableVoice, bool enableVideo);
    
    sigslot::signal2<RtcStream*, const std::string&> SignalSessionDescription;
    sigslot::signal2<RtcStream*, const std::string&> SignalIceCandidate;

protected:
    //  
    // PeerConnectionObserver implementation.
    //  
    virtual void OnError() {
        LOG(INFO) << __FUNCTION__ ;
    }
    virtual void OnStateChange(
            webrtc::PeerConnectionObserver::StateType state_changed) {
        LOG(INFO) << __FUNCTION__ ;
    }
    virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
    virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
    virtual void OnRenegotiationNeeded() {
        LOG(INFO) << __FUNCTION__ ;
    }
    virtual void OnIceChange() {
        LOG(INFO) << __FUNCTION__ ;
    }
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
    
    //
    // Callback from CreateSessionDescriptionObserver
    //   
    virtual void OnLocalDescription(webrtc::SessionDescriptionInterface* desc); 

protected:
    std::string id_;
    webrtc::PeerConnectionFactoryInterface* factory_;
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> connection_;
    SimpleVideoRenderer* renderer_;
};

#endif
