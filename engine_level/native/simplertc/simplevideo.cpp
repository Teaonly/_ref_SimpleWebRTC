#include <iostream>
#include <iostream>                                                                                                                                                                                           
#include "talk/media/base/rtputils.h"
#include "talk/base/buffer.h"
#include "talk/base/byteorder.h"
#include "talk/base/stringutils.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_header_parser.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp.h"

#include "simplevideo.h"

enum {
    MSG_RTP_TIMER,
};

namespace cricket {

class SimpleVideoRtcpFeedback : public webrtc::RtcpFeedback, public webrtc::RtpFeedback, public webrtc::RtcpIntraFrameObserver {
public:
    void SetModule(webrtc::RtpRtcp* module) {
        _rtpRtcpModule = module;
    };  
    // callbacks from RtcpFeedback
    virtual void OnApplicationDataReceived(const int32_t id, 
            const uint8_t subType,
            const uint32_t name,
            const uint16_t length,
            const uint8_t* data) {
        std::cout << ">>>>>>>>>>>>>>> OnApplicationDataReceived >>>>>>> " << std::endl;
    }  
    virtual void OnXRVoIPMetricReceived(
            const int32_t id, 
            const webrtc::RTCPVoIPMetric* metric) {
        std::cout << ">>>>>>>>>>>>>>> OnXRVoIPMetricReceived >>>>>>> " << std::endl;
    } 
    virtual void OnRTCPPacketTimeout(const int32_t id) {
        std::cout << ">>>>>>>>>>>>>>> OnRTCPPacketTimeout >>>>>>> " << std::endl;
    }
    virtual void OnSendReportReceived(const int32_t id, 
            const uint32_t senderSSRC,
            uint32_t ntp_secs,
            uint32_t ntp_frac,
            uint32_t timestamp) {
        std::cout << ">>>>>>>>>>>>>>> OnSendReportReceived >>>>>>> " << std::endl;
    }  
    virtual void OnReceiveReportReceived(const int32_t id, 
            const uint32_t senderSSRC) {
        std::cout << ">>>>>>>>>>>>>>>>>> OnReceiveReportReceived >>>>> " << std::endl;
    }  
    
    // callbacks from RtpFeedback
    virtual int32_t OnInitializeDecoder(
            const int32_t id,                                                                
            const int8_t payloadType,
            const char payloadName[RTP_PAYLOAD_NAME_SIZE],                                   
            const int frequency,
            const uint8_t channels,                                                          
            const uint32_t rate) {
        return 0;
    } 
    virtual void OnPacketTimeout(const int32_t id) {
    }
    virtual void OnReceivedPacket(const int32_t id,                                      
            const webrtc::RtpRtcpPacketType packetType) {
        std::cout << "KAKA" << std::endl;
    }
    virtual void OnPeriodicDeadOrAlive(const int32_t id,
            const webrtc::RTPAliveType alive) {
    }
    virtual void OnIncomingSSRCChanged( const int32_t id,                                
            const uint32_t SSRC) {
    }
    virtual void OnIncomingCSRCChanged( const int32_t id,                                
            const uint32_t CSRC,                             
            const bool added) {
    } 

    // callbacks RtcpIntraFrameObserver
    virtual void OnReceivedIntraFrameRequest(uint32_t ssrc) {
        std::cout << "============== OnReceivedIntraFrameRequest =======" << std::endl;
    };  
    virtual void OnReceivedSLI(uint32_t ssrc,
            uint8_t pictureId) {
        std::cout << "============== OnReceivedSLI  =======" << std::endl;
    };  
    virtual void OnReceivedRPSI(uint32_t ssrc,
            uint64_t pictureId) {
        std::cout << "============== OnReceivedSLI =======" << std::endl;
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
    
    module_process_thread_ = webrtc::ProcessThread::CreateProcessThread();    
    module_process_thread_->Start();

    //
    // Create and setup RTP/RTCP module 
    //
    rtcp_feedback_ = new SimpleVideoRtcpFeedback();

    webrtc::RtpRtcp::Configuration configuration;
    configuration.id = 1;
    configuration.clock = NULL;
    configuration.audio = false;
    configuration.outgoing_transport = this;
    configuration.incoming_data = this;
    configuration.incoming_messages = rtcp_feedback_;
    configuration.rtcp_feedback = rtcp_feedback_;
    configuration.intra_frame_callback = rtcp_feedback_;
    /*
    configuration.bandwidth_callback = bandwidth_observer;
    configuration.rtt_observer = rtt_observer; 
    configuration.remote_bitrate_estimator = remote_bitrate_estimator;
    configuration.paced_sender = paced_sender;
    */

    rtp_rtcp_module_ = webrtc::RtpRtcp::CreateRtpRtcp(configuration);
    rtcp_feedback_->SetModule( rtp_rtcp_module_);

    rtp_rtcp_module_->SetRTCPStatus(webrtc::kRtcpCompound);
    rtp_rtcp_module_->SetKeyFrameRequestMethod(webrtc::kKeyFrameReqPliRtcp);
    rtp_rtcp_module_->SetNACKStatus(webrtc::kNackOff, 0);
    rtp_rtcp_module_->SetSendingStatus(true);
    
    webrtc::VideoCodec video_codec;
    memset(&video_codec, 0, sizeof(video_codec));
    video_codec.plType = 100;
    memcpy(video_codec.plName, "VP8", 4);
    rtp_rtcp_module_->RegisterSendPayload(video_codec);
    rtp_rtcp_module_->RegisterReceivePayload(video_codec);
      
    module_process_thread_->RegisterModule(rtp_rtcp_module_); 

    rtp_header_parser_ =  webrtc::RtpHeaderParser::Create();
}

SimpleVideoEngine::~SimpleVideoEngine() {
    encoding_thread_->Clear(this);
    encoding_thread_->Quit();
    delete encoding_thread_;
    
    module_process_thread_->DeRegisterModule(rtp_rtcp_module_);
    webrtc::ProcessThread::DestroyProcessThread(module_process_thread_);
    
    delete rtp_header_parser_;
    delete rtp_rtcp_module_; 
    delete rtcp_feedback_;
}

void SimpleVideoEngine::OnMessage(talk_base::Message* msg) {
    switch( msg->message_id) {
        case MSG_RTP_TIMER:
            //SignalSendPacket(this, 0, 0);
            //encoding_thread_->PostDelayed(100, this, MSG_RTP_TIMER);
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

  // Implements RtpData.
int32_t SimpleVideoEngine::OnReceivedPayloadData(
        const uint8_t* payload_data,
        const uint16_t payload_size,
        const webrtc::WebRtcRTPHeader* rtp_header) {


    return 0; 
}


bool SimpleVideoEngine::InsertRtcpPackage(unsigned char* data, unsigned int len) {
    rtp_rtcp_module_->IncomingRtcpPacket(data, len);
    return true;
}

bool SimpleVideoEngine::InsertRtpPackage(unsigned char* data, unsigned int len) {
    webrtc::RTPHeader header;
    if (!rtp_header_parser_->Parse(data, len, &header)) {
        return false;
    }
    rtp_rtcp_module_->IncomingRtpPacket(data, len, header);        
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
    if (0) {
        static uint8 buf[2048]; 
        const void* data = packet->data();
        size_t len = packet->length();
        memcpy(buf, data, len);
        
        engine_->InsertRtpPackage(buf, len);
    } 
    
    if (1) {
        //  This is a simple loopback
        static uint8 buf[2048]; 
        const void* data = packet->data();
        size_t len = packet->length();
        memcpy(buf, data, len);
        SetRtpSsrc(buf, len, target_ssrc_);
        talk_base::Buffer new_packet((const void*)buf, len, 2048);
        network_interface()->SendPacket(&new_packet);
    }
    
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
    /*
    if ( options == options_ ) {
        return true;
    }
    */

    return true;
}

void SimpleVideoMediaChannel::UpdateAspectRatio(int ratio_w, int ratio_h) {
} 

bool SimpleVideoMediaChannel::SetRecvRtpHeaderExtensions(
     const std::vector<cricket::RtpHeaderExtension>& extensions) { 
    return true; 
}

bool SimpleVideoMediaChannel::SetSendRtpHeaderExtensions(
     const std::vector<cricket::RtpHeaderExtension>& extensions) {
#if 1
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
    /*
    if ( (rand() % 100) == 1) {
        std::cout << "KAKAK" << std::endl;
    }
    */
    static uint8 buf[2048];
    memcpy(buf, data, len);
    SetRtpSsrc(buf, len, target_ssrc_);
    talk_base::Buffer new_packet((const void*)buf, len, 2048);
    network_interface()->SendPacket(&new_packet);
}

void SimpleVideoMediaChannel::OnSendRTCPPacket(SimpleVideoEngine *eng, const void *data, int len) {
    uint8 buf[2048];
    memcpy(&buf[4], data, len);
    SetRtpSsrc(buf, len, target_ssrc_);
    talk_base::Buffer new_packet((const void*)&buf[4], len, 1024);
    network_interface()->SendRtcp(&new_packet);
}

}   // end of namespace 
