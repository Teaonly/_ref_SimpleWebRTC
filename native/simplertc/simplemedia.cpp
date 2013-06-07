#include <iostream>

#include "simplemedia.h"
#include "simplevoice.h"
#include "simplevideo.h"

namespace cricket {

SimpleMediaEngine::SimpleMediaEngine(talk_base::Thread* worker_thread)
        : worker_thread_(worker_thread){
    // create database for media codecs        
    cricket::VideoCodec codec;
    codec.name = "VP8";
    codec.id = 100;          
    codec.width = 640; 
    codec.height = 480;
    codec.framerate = 30;     
    codec.preference = 0;
    video_codecs_.push_back(codec); 

    cricket::AudioCodec isac(103, "ISAC", 16000, 32000, 1, 1);
    audio_codecs_.push_back(isac);
}

SimpleMediaEngine::~SimpleMediaEngine() {

}

bool SimpleMediaEngine::Init(talk_base::Thread* worker_thread) {
    return true;
}

void SimpleMediaEngine::Terminate() {

}

int SimpleMediaEngine::GetCapabilities() {
    //return VIDEO_RECV | VIDEO_SEND | AUDIO_RECV | AUDIO_SEND;
    return VIDEO_SEND | VIDEO_RECV | AUDIO_RECV | AUDIO_SEND;
}

VoiceMediaChannel* SimpleMediaEngine::CreateChannel() {
    std::cout << "[MEDIA]\tCreateVoiceMediaChannel" << std::endl; 
    SimpleVoiceMediaChannel* channel =
            new SimpleVoiceMediaChannel();
    return channel;   
}

VideoMediaChannel* SimpleMediaEngine::CreateVideoChannel(
        VoiceMediaChannel* voice_media_channel) {
    std::cout << "[MEDIA]\tCreateVideoMediaChannel" << std::endl;

    SimpleVideoEngine* video_engine = NULL;
    video_engine = new SimpleVideoEngine(); 
    SimpleVideoMediaChannel* channel = new SimpleVideoMediaChannel(video_engine, voice_media_channel, worker_thread_);

    return channel;
}

SoundclipMedia* SimpleMediaEngine::CreateSoundclip() {
    return NULL;
}

bool SimpleMediaEngine::SetAudioOptions(int options) {
    // AUDIO_TODO
    return true;
}

bool SimpleMediaEngine::SetVideoOptions(int options) {
    return true;
}

bool SimpleMediaEngine::SetAudioDelayOffset(int offset) {
    // AUDIO_TODO
    return true;
}

bool SimpleMediaEngine::SetDefaultVideoEncoderConfig(const VideoEncoderConfig& config) {
    std::cout << "[MEDIA]\tSetDefaultVideoEncoderConfig:   max_codec = " << config.max_codec.ToString() << std::endl; 
    return true;
}

bool SimpleMediaEngine::SetSoundDevices(const Device* in_device,
        const Device* out_device) {
    return true;
}

bool SimpleMediaEngine::SetVideoCapturer(VideoCapturer* capturer) {
    return true;
}

VideoCapturer* SimpleMediaEngine::GetVideoCapturer() const{
    return NULL;
}

bool SimpleMediaEngine::GetOutputVolume(int* level) {
    *level = 0;
    return true;
}

bool SimpleMediaEngine::SetOutputVolume(int level) {
    return true;
}

int SimpleMediaEngine::GetInputLevel() {
    return 0;
}

bool SimpleMediaEngine::SetLocalMonitor(bool enable) {
    return true;
}

bool SimpleMediaEngine::SetLocalRenderer(VideoRenderer* renderer) {
    return true;
}

bool SimpleMediaEngine::SetVideoCapture(bool capture) {
    return true;
}

const std::vector<RtpHeaderExtension>&
      SimpleMediaEngine::audio_rtp_header_extensions() {
    return audio_rtp_header_extensions_;
}

const std::vector<RtpHeaderExtension>&
      SimpleMediaEngine::video_rtp_header_extensions() {
    return video_rtp_header_extensions_;
}

const std::vector<AudioCodec>& SimpleMediaEngine::audio_codecs() {
    return audio_codecs_;
}

const std::vector<VideoCodec>& SimpleMediaEngine::video_codecs() {
    return video_codecs_;
}

void SimpleMediaEngine::SetVoiceLogging(int min_sev, const char* filter) {
    return;
}

void SimpleMediaEngine::SetVideoLogging(int min_sev, const char* filter) {
}

bool SimpleMediaEngine::RegisterVideoProcessor(VideoProcessor* video_processor) {
    return true;
}

bool SimpleMediaEngine::UnregisterVideoProcessor(VideoProcessor* video_processor) {
    return true;
}

bool SimpleMediaEngine::RegisterVoiceProcessor(uint32 ssrc,
        VoiceProcessor* video_processor,
        MediaProcessorDirection direction) {
    return true;
}

bool SimpleMediaEngine::UnregisterVoiceProcessor(uint32 ssrc,
        VoiceProcessor* video_processor,
        MediaProcessorDirection direction) {
    return true;
}

VideoFormat SimpleMediaEngine::GetStartCaptureFormat() const {
    return VideoFormat();
} 

}
