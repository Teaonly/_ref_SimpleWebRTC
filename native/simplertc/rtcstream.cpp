#include <iostream>
#include <sstream>

#include "talk/app/webrtc/videosourceinterface.h"
#include "rtcstream.h"
#include "talk/base/base64.h"
#include "talk/base/json.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

// 
// Internal helper observer implementation. 
//
class RtcStreamSetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
public:
    static RtcStreamSetSessionDescriptionObserver* Create() {
        return
            new talk_base::RefCountedObject<RtcStreamSetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {
        LOG(INFO) << "SetSessionDescription is Success";
    }
    virtual void OnFailure(const std::string& error) {
        LOG(WARNING) << "SetSessionDescription is failture";
    }

protected:
    RtcStreamSetSessionDescriptionObserver() {}
    ~RtcStreamSetSessionDescriptionObserver() {}

};

class RtcStreamCreateSessionDescriptionObserver
    : public webrtc::CreateSessionDescriptionObserver
    , public sigslot::has_slots<> {
public:
    static RtcStreamCreateSessionDescriptionObserver* Create() {
        return
            new talk_base::RefCountedObject<RtcStreamCreateSessionDescriptionObserver>();
    }
    
    virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) {
        SignalCreatedSession(desc);
    } 
    
    virtual void OnFailure(const std::string& error) {
        SignalCreatedSession(NULL);
    }
    
    sigslot::signal1<webrtc::SessionDescriptionInterface*> SignalCreatedSession;

protected:
    RtcStreamCreateSessionDescriptionObserver() {}
    ~RtcStreamCreateSessionDescriptionObserver() {}
};

RtcStream::RtcStream(const std::string& id, 
            webrtc::PeerConnectionFactoryInterface* factory, 
            cricket::VideoCapturer* vc, 
            webrtc::VideoRendererInterface* vr,
            webrtc::AudioSourceInterface* as):
            id_(id), factory_(factory),
            videoRenderer_(vr), audioSource_(as), videoCapturer_(vc) {

    webrtc::PeerConnectionInterface::IceServers servers;
    webrtc::PeerConnectionInterface::IceServer server;
    server.uri = "stun:stun.l.google.com:19302";
    servers.push_back(server);
    
    connection_ = factory_->CreatePeerConnection(servers, 
                                                 NULL,
                                                 NULL,
                                                 this); 
}

RtcStream::~RtcStream() {
    connection_->Close();
}

// PeerConnectionObserver callbackes
void RtcStream::OnAddStream(webrtc::MediaStreamInterface* stream) {
    
    stream->AddRef();
    webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
    // Only render the first track.
    if (!tracks.empty()) {
        webrtc::VideoTrackInterface* track = tracks[0];
        if ( videoRenderer_ != NULL) {
            track->AddRenderer(videoRenderer_);
        }
    }
    stream->Release();
}

void RtcStream::OnRemoveStream(webrtc::MediaStreamInterface* stream) {
    LOG(INFO) << __FILE__ << "@" << __LINE__ << "OnRemoveStream";
}

void RtcStream::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    std::string cand;
    if (!candidate->ToString(&cand)) {
        LOG(LS_ERROR) << "Failed to serialize candidate";
        return;
    }

    Json::StyledWriter writer;
    Json::Value jmessage;
    jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
    jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
    jmessage[kCandidateSdpName] = cand;
    std::string encodedCand = talk_base::Base64::Encode( writer.write(jmessage)); 
    SignalIceCandidate(this, encodedCand);
}

void RtcStream::OnLocalDescription(webrtc::SessionDescriptionInterface* desc) {
    if ( desc != NULL) {
        connection_->SetLocalDescription(    
                RtcStreamSetSessionDescriptionObserver::Create(), desc);
    
        Json::StyledWriter writer;
        Json::Value jmessage;
        jmessage[kSessionDescriptionTypeName] = desc->type();
        std::string sdp;
        desc->ToString(&sdp);
        jmessage[kSessionDescriptionSdpName] = sdp;

        std::string encodedDesc = talk_base::Base64::Encode( writer.write(jmessage) );
        SignalSessionDescription(this, encodedDesc); 
    } else {
        LOG(WARNING) << "Create local sessiondescription error!"; 
    }    
} 

void RtcStream::SetRemoteCandidate(const std::string& msg) {
    // convert to local json object
    Json::Reader reader;
    Json::Value jmessage;
    std::string decodedMsg = talk_base::Base64::Decode(msg, talk_base::Base64::DO_STRICT);     
    if (!reader.parse(decodedMsg, jmessage)) {
        LOG(WARNING) << " Parse the JSON failed";
        return;
    }

    // parse the received json object
    std::string sdp_mid;
    int sdp_mlineindex = 0;
    std::string sdp;
    if (!GetStringFromJsonObject(jmessage, kCandidateSdpMidName, &sdp_mid) ||
            !GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName,
                    &sdp_mlineindex) ||
            !GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) {
        LOG(WARNING) << "Can't parse received message.";
        return;
    }   
    
    talk_base::scoped_ptr<webrtc::IceCandidateInterface> candidate( 
                webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp));
    if (!candidate.get()) {
        LOG(WARNING) << "Can't parse received candidate message.";
        return;
    }   
    
    if (!connection_->AddIceCandidate(candidate.get())) {
        LOG(WARNING) << "Failed to apply the received candidate";
        return;
    }   
    
    return;
}

void RtcStream::SetRemoteDescription(const std::string& msg) {
    // convert to local json object
    Json::Reader reader;
    Json::Value jmessage;
    std::string decodedMsg = talk_base::Base64::Decode(msg, talk_base::Base64::DO_STRICT);     
    if (!reader.parse(decodedMsg, jmessage)) {
        LOG(WARNING) << " Parse the JSON failed";
        return;
    }
    // convert to local sesion description object
    std::string type;
    std::string sdp;
    GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);
    GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName, &sdp);
    if ( sdp.empty() || type.empty() ) {
        LOG(WARNING) << " Convert to SDP failed";
        return;
    }

    webrtc::SessionDescriptionInterface* session_description(
            webrtc::CreateSessionDescription(type, sdp));
    if (!session_description) {
        LOG(WARNING) << "Can't parse received session description message."; 
        return;
    }

    connection_->SetRemoteDescription(
            RtcStreamSetSessionDescriptionObserver::Create(), session_description);
}

static cricket::VideoCapturer* OpenVideoCaptureDevice() {
  talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
      cricket::DeviceManagerFactory::Create());
  if (!dev_manager->Init()) {
    LOG(LS_ERROR) << "Can't create device manager";
    return NULL;
  }
  std::vector<cricket::Device> devs;
  if (!dev_manager->GetVideoCaptureDevices(&devs)) {
    LOG(LS_ERROR) << "Can't enumerate video devices";
    return NULL;
  }
  std::vector<cricket::Device>::iterator dev_it = devs.begin();
  cricket::VideoCapturer* capturer = NULL;
  for (; dev_it != devs.end(); ++dev_it) {
    capturer = dev_manager->CreateVideoCapturer(*dev_it);
    if (capturer != NULL)
      break;
  }
  return capturer;
}

void RtcStream::SetupLocalStream(bool enableVoice, bool enableVideo) {
    if ( enableVoice == false && enableVideo == false)
        return;

    talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream = 
            factory_->CreateLocalMediaStream("simple_stream");

    if ( enableVoice && audioSource_ != NULL) {
        talk_base::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
                factory_->CreateAudioTrack(
                    "simple_voice", audioSource_));        
        stream->AddTrack(audio_track);
    }

    //videoCapturer_ = OpenVideoCaptureDevice();
    if ( enableVideo && videoCapturer_ != NULL) {
        talk_base::scoped_refptr<webrtc::VideoSourceInterface> video_source(
            factory_->CreateVideoSource(videoCapturer_, NULL));
        talk_base::scoped_refptr<webrtc::VideoTrackInterface> video_track(
                factory_->CreateVideoTrack(
                    "simple_video", video_source));
        stream->AddTrack(video_track);
    }

    connection_->AddStream(stream, NULL);    
}

void RtcStream::CreateOfferDescription() {
    RtcStreamCreateSessionDescriptionObserver *ob = RtcStreamCreateSessionDescriptionObserver::Create();
    ob->SignalCreatedSession.connect(this, &RtcStream::OnLocalDescription);
    connection_->CreateOffer(ob, NULL);
}

void RtcStream::CreateAnswerDescription() {
    RtcStreamCreateSessionDescriptionObserver *ob = RtcStreamCreateSessionDescriptionObserver::Create();
    ob->SignalCreatedSession.connect(this, &RtcStream::OnLocalDescription);
    connection_->CreateAnswer(ob, NULL);
}

