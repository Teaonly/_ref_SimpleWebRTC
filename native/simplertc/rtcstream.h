#ifndef _RTCSTREAM_H_
#define _RTCSTREAM_H_

#include <string>
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/thread.h"
#include "talk/session/media/channelmanager.h"

#include "fakedtlsidentityservice.h"

class Peer;

class RtcStream : public webrtc::PeerConnectionObserver, 
                  public sigslot::has_slots<> {
public:
    RtcStream(const std::string& id, webrtc::PeerConnectionFactoryInterface* factory, 
            cricket::VideoCapturer* vc, 
            webrtc::VideoRendererInterface* vr);
    virtual ~RtcStream();
    
    inline std::string id() { return id_; }
    
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
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);
    virtual void OnAddStream(webrtc::MediaStreamInterface* stream);
    virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream);
     
    virtual void OnRenegotiationNeeded() {}
    virtual void OnStateChange(
            webrtc::PeerConnectionObserver::StateType state_changed) {}
    virtual void OnIceConnectionChange(
            webrtc::PeerConnectionInterface::IceConnectionState new_state) {}
    virtual void OnIceGatheringChange(
            webrtc::PeerConnectionInterface::IceGatheringState new_state) {}
    virtual void OnError() {}

    //
    // Callback from CreateSessionDescriptionObserver
    //   
    virtual void OnLocalDescription(webrtc::SessionDescriptionInterface* desc); 

protected:
    std::string id_;

    FakeIdentityService myDTLS;

    webrtc::PeerConnectionFactoryInterface* factory_;

    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> connection_;
    
    cricket::VideoCapturer* videoCapturer_;
    webrtc::VideoRendererInterface* videoRenderer_;
};

#endif
