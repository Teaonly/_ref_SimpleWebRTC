#include <iostream>
#include <iostream>                                                                                                                                                                                           
#include "talk/media/base/rtputils.h"
#include "talk/base/buffer.h"
#include "talk/base/byteorder.h"
#include "talk/base/stringutils.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp.h"

#include "simplevideo.h"

enum {
    MSG_RTP_TIMER,
};

namespace cricket {
class SimpleVideoRtcpFeedback : public webrtc::RtcpFeedback, public webrtc::RtcpIntraFrameObserver {
public:
    void SetModule(webrtc::RtpRtcp* module) {
        _rtpRtcpModule = module;
    };  
    virtual void OnRTCPPacketTimeout(const int32_t id) {
    }
    virtual void OnLipSyncUpdate(const int32_t id, 
            const int32_t audioVideoOffset) {
    };  
    virtual void OnXRVoIPMetricReceived(
            const int32_t id, 
            const webrtc::RTCPVoIPMetric* metric) {
    };  
    virtual void OnApplicationDataReceived(const int32_t id, 
            const uint8_t subType,
            const uint32_t name,
            const uint16_t length,
            const uint8_t* data) {
    };  
    virtual void OnSendReportReceived(const int32_t id, 
            const uint32_t senderSSRC,
            uint32_t ntp_secs,
            uint32_t ntp_frac,
            uint32_t timestamp) {
    };  
    virtual void OnReceiveReportReceived(const int32_t id, 
            const uint32_t senderSSRC) {
    };  
    virtual void OnReceivedIntraFrameRequest(uint32_t ssrc) {
    };  
    virtual void OnReceivedSLI(uint32_t ssrc,
            uint8_t pictureId) {
    };  
    virtual void OnReceivedRPSI(uint32_t ssrc,
            uint64_t pictureId) {
    };  
    virtual void OnLocalSsrcChanged(uint32_t old_ssrc, uint32_t new_ssrc) {
    }; 
private:
    webrtc::RtpRtcp* _rtpRtcpModule;
};  


SimpleVideoEngine::SimpleVideoEngine() {
    //
    //  basic setup
    //
    isSend_ = false;
    encoding_thread_ = new talk_base::Thread();
    encoding_thread_->Start();

    //
    // Create and setup RTP/RTCP module 
    //
    rtcp_feedback_ = new SimpleVideoRtcpFeedback();

    webrtc::RtpRtcp::Configuration configuration;
    configuration.id = 1;
    configuration.audio = false;
    configuration.clock = NULL;
    configuration.outgoing_transport = this;
    configuration.rtcp_feedback = rtcp_feedback_;
    configuration.intra_frame_callback = rtcp_feedback_;
    rtp_rtcp_module_ = webrtc::RtpRtcp::CreateRtpRtcp(configuration);
    rtcp_feedback_->SetModule( rtp_rtcp_module_);
    
    rtp_rtcp_module_->SetRTCPStatus(webrtc::kRtcpCompound);
    rtp_rtcp_module_->SetKeyFrameRequestMethod(webrtc::kKeyFrameReqPliRtcp);
    rtp_rtcp_module_->SetSendingStatus(true);
    
    webrtc::VideoCodec video_codec;
    memset(&video_codec, 0, sizeof(video_codec));
    video_codec.plType = 100;
    memcpy(video_codec.plName, "VP8", 4);
    rtp_rtcp_module_->RegisterSendPayload(video_codec);
    rtp_rtcp_module_->RegisterReceivePayload(video_codec);
}

SimpleVideoEngine::~SimpleVideoEngine() {
    encoding_thread_->Clear(this);
    encoding_thread_->Quit();
    delete encoding_thread_;

    delete rtp_rtcp_module_; 
    delete rtcp_feedback_;  
}

void SimpleVideoEngine::OnMessage(talk_base::Message* msg) {
    switch( msg->message_id) {
        case MSG_RTP_TIMER:
            SignalSendPacket(this, 0, 0);
            encoding_thread_->PostDelayed(100, this, MSG_RTP_TIMER);
            break;
    }
}


void SimpleVideoEngine::StartSend(bool send) {
    isSend_ = send;
    if ( isSend_ == true) {
        encoding_thread_->PostDelayed(1000, this, MSG_RTP_TIMER); 
    }
}

int SimpleVideoEngine::SendPacket(int channel, const void *data, int len) {
    SignalSendPacket(this, data, len);
    return len;
}

int SimpleVideoEngine::SendRTCPPacket(int channel, const void *data, int len) {
    SignalSendRTCPPacket(this, data, len);
    return len;
}

bool SimpleVideoEngine::InsertRtcpPackage(unsigned char* data, unsigned int len) {
    //unsigned int ssrc;
    //GetRtpSsrc(data,len,&ssrc);
    rtp_rtcp_module_->IncomingPacket(data, len);
    rtp_rtcp_module_->Process();
    return true;
}

//
//
//  channel object is the owner of engine
//
//
SimpleVideoMediaChannel::SimpleVideoMediaChannel(
        SimpleVideoEngine* engine, 
        cricket::VoiceMediaChannel* channel, 
        talk_base::Thread* thread)
        : engine_(engine), 
          worker_thread_(thread), 
          voice_channel_(channel) { 

    engine_->SignalSendPacket.connect(this, &SimpleVideoMediaChannel::OnSendPacket);
    engine_->SignalSendRTCPPacket.connect(this, &SimpleVideoMediaChannel::OnSendRTCPPacket);

    target_ssrc_ = 0;
}

bool SimpleVideoMediaChannel::Init() {
    return true;
}

SimpleVideoMediaChannel::~SimpleVideoMediaChannel() {
    delete engine_;    
}

void SimpleVideoMediaChannel::OnMessage(talk_base::Message *msg) { 
    switch( msg->message_id ) {
    }
}

bool SimpleVideoMediaChannel::SetRecvCodecs(
    const std::vector<cricket::VideoCodec>& codecs) {
    std::cout << "[VIDEO]\tSetRecvCodecs : " << std::endl;
    for (int i = 0; i < (int)codecs.size(); i++ ) {
        std::cout << "\t " << i << ". name = " << codecs[i].name << " id = " << codecs[i].id << " width = " << codecs[i].width << std::endl; 
        for( std::map<std::string, std::string>::const_iterator j = codecs[i].params.begin(); 
                j != codecs[i].params.end(); j++) {
            std::cout << "\t\t\t" << j->first << " := " << j->second  << std::endl;
        }
    }
    return true;
}

bool SimpleVideoMediaChannel::SetSendCodecs(
    const std::vector<cricket::VideoCodec>& codecs) {
    std::cout << "[VIDEO]\tSetSendCodecs : "<< std::endl;
    for (int i = 0; i < (int)codecs.size(); i++ ) {
        std::cout << "\t " << i << ". name = " << codecs[i].name << " id = " << codecs[i].id << " width = " << codecs[i].width << std::endl; 
        for( std::map<std::string, std::string>::const_iterator j = codecs[i].params.begin(); 
                j != codecs[i].params.end(); j++) {
            std::cout << "\t\t\t" << j->first << " := " << j->second  << std::endl;
        }
    }
    return true;
}

bool SimpleVideoMediaChannel::SetRender(bool render) {
    // We don't care render's status, just return true 
    return true;
}

bool SimpleVideoMediaChannel::SetSend(bool send) {
    std::cout << "[VIDEO]\tSetSend = " << send << std::endl;
    engine_->StartSend(send);
    return true;
}

void SimpleVideoMediaChannel::OnReadyToSend(bool send) {
    std::cout << "[VIDEO]\tOnReadyToSend = " << send << std::endl;
    engine_->StartSend(send);
}

bool SimpleVideoMediaChannel::SetRenderer(
    uint32 ssrc, cricket::VideoRenderer* renderer) {
    // We don't care renderer, just return true
    return true;
}

bool SimpleVideoMediaChannel::GetStats(cricket::VideoMediaInfo* info) {
    return true;
}

bool SimpleVideoMediaChannel::SendIntraFrame() {
    // not apply in WebRTC
    return true;
}

bool SimpleVideoMediaChannel::RequestIntraFrame() {
    // not apply in WebRTC
    return true;
}

void SimpleVideoMediaChannel::OnPacketReceived(talk_base::Buffer* packet) {
#if 1
    //  This is a simple loopback
    static uint8 buf[2048]; 
    const void* data = packet->data();
    size_t len = packet->length();
    memcpy(buf, data, len);
    SetRtpSsrc(buf, len, target_ssrc_);
    talk_base::Buffer new_packet((const void*)buf, len, 2048);
    network_interface()->SendPacket(&new_packet);
#endif
    if (0) {
        static FILE *fp = NULL;
        static unsigned short seq = 0;
        static uint8 buf[2048];
        if ( fp == NULL) {
            fp = fopen("./vp8.rtp", "rb");
        }
        if ( feof(fp) ) {
            fseek(fp, 0l, SEEK_SET);
        }
        fread(buf, 1, 2048, fp);
        size_t len = *(size_t *)&buf[2040];
        SetRtpSsrc(buf, len, target_ssrc_);
        SetRtpSeqNum(buf, len, seq++);
        talk_base::Buffer new_packet((const void*)buf, len, 2048);
        network_interface()->SendPacket(&new_packet);
    }
}

void SimpleVideoMediaChannel::OnRtcpReceived(talk_base::Buffer* packet) {
    const void* data = packet->data();
    size_t len = packet->length();
    uint8 buf[2048];
    memcpy(buf, data, len);
    talk_base::Buffer new_packet((const void*)buf, len, 2048);
    engine_->InsertRtcpPackage(buf, len);
}

bool SimpleVideoMediaChannel::SetSendBandwidth(bool autobw, int bps) {
    // TODO
    return true;
}

bool SimpleVideoMediaChannel::SetOptions(const VideoOptions& options) {
    if ( options == options_ ) {
        return true;
    }
    
    return false;
}

void SimpleVideoMediaChannel::UpdateAspectRatio(int ratio_w, int ratio_h) {
} 

bool SimpleVideoMediaChannel::SetRecvRtpHeaderExtensions(
     const std::vector<cricket::RtpHeaderExtension>& extensions) { 
    return true; 
}

bool SimpleVideoMediaChannel::SetSendRtpHeaderExtensions(
     const std::vector<cricket::RtpHeaderExtension>& extensions) {
#if 0
    // we must apply the extensions, otherwise setRemoteDescription will return error.
    // TODO apply these RTP extensions
    std::cout << "[VIDEO]\tSendRtpHeaderExtensions : " << extensions.size() << std::endl;
#endif
    return true; 
}

bool  SimpleVideoMediaChannel::AddSendStream(const cricket::StreamParams& sp) {
    std::cout << "[VIDEO]\tAddSendStream : " << sp.ToString() << std::endl;
    return true;
}

bool  SimpleVideoMediaChannel::RemoveSendStream(uint32 ssrc){
    // TODO
    return true;
}

bool  SimpleVideoMediaChannel::AddRecvStream(const cricket::StreamParams& sp) {
    std::cout << "[VIDEO]\tAddRecvStream : " << sp.ToString() << std::endl;
    return true;
}

bool  SimpleVideoMediaChannel::RemoveRecvStream(uint32 ssrc) {
    // TODO
    return true;
}

bool  SimpleVideoMediaChannel::MuteStream(uint32 ssrc, bool on) {
    std::cout << "[VIDEO]\tMuteStream : ssrc = " << ssrc << " on = " << on << std::endl;

    target_ssrc_ = ssrc;
    return true;
}

bool  SimpleVideoMediaChannel::GetSendCodec(cricket::VideoCodec* codec) {
    // not apply in WebRTC
    return true;
}

bool  SimpleVideoMediaChannel::SetSendStreamFormat(uint32 ssrc, const cricket::VideoFormat& format) {
    // not apply in WebRTC
    return true;
}

bool SimpleVideoMediaChannel::SetCapturer(uint32 ssrc, cricket::VideoCapturer* capturer) {
    // We don't care capturer
    return true;
}

bool SimpleVideoMediaChannel::GetOptions(VideoOptions* options) const {
    *options = options_;
    return true;
}

void SimpleVideoMediaChannel::OnSendPacket(SimpleVideoEngine *eng, const void *data, int len) {
#if 0
    static uint8 buf[2048];
    memcpy(buf, data, len);
    SetRtpSsrc(buf, len, target_ssrc_);
    talk_base::Buffer new_packet((const void*)buf, len, 2048);
    network_interface()->SendPacket(&new_packet);
#else
    //if ( data == NULL) {
    if (0) {
        static FILE *fp = NULL;
        static unsigned short seq = 0;
        static uint8 buf[2048];
        if ( fp == NULL) {
            fp = fopen("./vp8.rtp", "rb");
        }
        if ( feof(fp) ) {
            fseek(fp, 0l, SEEK_SET);
        }
        fread(buf, 1, 2048, fp);
        size_t len = *(size_t *)&buf[2040];
        SetRtpSsrc(buf, len, target_ssrc_);
        SetRtpSeqNum(buf, len, seq++);
        talk_base::Buffer new_packet((const void*)buf, len, 2048);
        network_interface()->SendPacket(&new_packet);
    }
#endif
}

void SimpleVideoMediaChannel::OnSendRTCPPacket(SimpleVideoEngine *eng, const void *data, int len) {
    uint8 buf[2048];
    memcpy(&buf[4], data, len);
    SetRtpSsrc(buf, len, target_ssrc_);
    talk_base::Buffer new_packet((const void*)&buf[4], len, 1024);
    network_interface()->SendRtcp(&new_packet);
}

}   // end of namespace 
