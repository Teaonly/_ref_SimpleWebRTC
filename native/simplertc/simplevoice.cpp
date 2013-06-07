#include <iostream>
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp_defines.h"
#include "webrtc/modules/rtp_rtcp/interface/rtp_rtcp.h"
#include "webrtc/common_types.h"
#include "webrtc/modules/audio_coding/main/interface/audio_coding_module.h"

#include "simplevoice.h"

using namespace cricket;

SimpleVoiceEngine::SimpleVoiceEngine() {
    //
    //  basic setup
    //
    isSend_ = false;
    WebRtcIsac_Create(&encoder_);
    WebRtcIsac_EncoderInit(encoder_, 0);    
    encoding_thread_ = new talk_base::Thread();
    encoding_thread_->Start();
    
    //
    //  RTCP module setup
    //
    webrtc::RtpRtcp::Configuration configuration;
    configuration.id = 2;
    configuration.audio = true;
    configuration.clock = NULL;
    configuration.outgoing_transport = this;
    configuration.rtcp_feedback = this;
    rtp_rtcp_module_ = webrtc::RtpRtcp::CreateRtpRtcp(configuration);
    
    rtp_rtcp_module_->SetRTCPStatus(webrtc::kRtcpCompound);
    rtp_rtcp_module_->SetSendingStatus(true);
    
    const uint8_t nSupportedCodecs = webrtc::AudioCodingModule::NumberOfCodecs();
    for (int idx = 0; idx < nSupportedCodecs; idx++) {
        webrtc::CodecInst codec;
        if (webrtc::AudioCodingModule::Codec(idx, &codec) == -1) {
            continue;
        }
        if ( codec.pltype == 103) {
            rtp_rtcp_module_->RegisterSendPayload(codec);
            break;
        }
    }
}

SimpleVoiceEngine::~SimpleVoiceEngine() {
    encoding_thread_->Clear(this);
    encoding_thread_->Quit();
    delete encoding_thread_;

    WebRtcIsac_Free(encoder_);
    encoder_ = NULL;

    delete rtp_rtcp_module_;
}

void SimpleVoiceEngine::OnMessage(talk_base::Message *msg) {

}

//------------------------------------------

SimpleVoiceMediaChannel::SimpleVoiceMediaChannel() {
    engine_ = new SimpleVoiceEngine();
    
}

SimpleVoiceMediaChannel::~SimpleVoiceMediaChannel() {
    delete engine_; 
}


bool SimpleVoiceMediaChannel::SetRecvCodecs(const std::vector<AudioCodec>& codecs) {
    return true;
}

bool SimpleVoiceMediaChannel::SetSendCodecs(const std::vector<AudioCodec>& codecs) {
    return true;
}

bool SimpleVoiceMediaChannel::SetSend(cricket::SendFlags flag) {
    return true;
}

bool SimpleVoiceMediaChannel::SetPlayout(bool playout) {
    return true;
}

void SimpleVoiceMediaChannel::OnReadyToSend(bool flag) {
    // TODO 
}

void SimpleVoiceMediaChannel::GetLastMediaError(uint32* ssrc, VoiceMediaChannel::Error* error) {
    ASSERT(error != NULL);
    *error = ERROR_NONE;
}

bool SimpleVoiceMediaChannel::AddSendStream(const cricket::StreamParams& sp){
    target_ssrc_ = sp.first_ssrc();        
    return true;
}

bool SimpleVoiceMediaChannel::RemoveSendStream(uint32 ssrc) {
    return true;
}

bool SimpleVoiceMediaChannel::AddRecvStream(const cricket::StreamParams& sp) {
    return true;
}

bool SimpleVoiceMediaChannel::RemoveRecvStream(uint32) { 
    return true;
}

bool SimpleVoiceMediaChannel::MuteStream(uint32 ssrc, bool isMute) { 
    return true;
}
 


