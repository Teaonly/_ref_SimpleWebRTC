#include <iostream>
#include "simplevoice.h"

using namespace cricket;

SimpleVoiceEngine::SimpleVoiceEngine() {

}

SimpleVoiceEngine::~SimpleVoiceEngine() {

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
 


