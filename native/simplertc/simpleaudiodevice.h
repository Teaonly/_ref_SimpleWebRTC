#ifndef _SIMPLEAUDIODEVICE_H_
#define _SIMPLEAUDIODEVICE_H_

#include "webrtc/modules/audio_device/include/audio_device.h"

namespace webrtc {

class CriticalSectionWrapper;

class SimpleAudioDevice : public AudioDeviceModule {
public:
    SimpleAudioDevice();
    virtual ~SimpleAudioDevice();

public: // Interface
    void Test();

public: // RefCountedModule
    virtual int32_t TimeUntilNextProcess() OVERRIDE;
    virtual int32_t Process() OVERRIDE;

public: // AudioDeviceModule
    
    // Retrieve the currently utilized audio layer
    virtual int32_t ActiveAudioLayer(AudioLayer* audioLayer) const {
        return AudioDeviceModule::kPlatformDefaultAudio;   
    }

    // Error handling
    virtual ErrorCode LastError() const { return kAdmErrNone; }
    virtual int32_t RegisterEventObserver(AudioDeviceObserver* eventCallback);
    
    // Full-duplex transportation of PCM audio
    virtual int32_t RegisterAudioCallback(AudioTransport* audioCallback);

    // Main initialization and termination
    virtual int32_t Init() { return 0; }
    virtual int32_t Terminate() { return 0; }
    virtual bool Initialized() const { return true; }

    // Device enumeration
    virtual int16_t PlayoutDevices() { return 0; }
    virtual int16_t RecordingDevices() { return 0; }
    virtual int32_t PlayoutDeviceName(uint16_t index,
                            char name[kAdmMaxDeviceNameSize],
                            char guid[kAdmMaxGuidSize]) {
        return 0;
    }
    virtual int32_t RecordingDeviceName(uint16_t index,
                              char name[kAdmMaxDeviceNameSize],
                              char guid[kAdmMaxGuidSize]) {
        return 0;
    }

    // Device selection
    virtual int32_t SetPlayoutDevice(uint16_t index) { return 0; }
    virtual int32_t SetPlayoutDevice(WindowsDeviceType device) { return 0; }
    virtual int32_t SetRecordingDevice(uint16_t index) { return 0; }
    virtual int32_t SetRecordingDevice(WindowsDeviceType device) { return 0; }
    
    // Audio transport initialization
    virtual int32_t PlayoutIsAvailable(bool* available) { return 0; }
    virtual int32_t InitPlayout() { return 0; }
    virtual bool PlayoutIsInitialized() const { return true; }
    virtual int32_t RecordingIsAvailable(bool* available) { return 0; }
    virtual int32_t InitRecording() { return 0; }
    virtual bool RecordingIsInitialized() const { return true; }

    // Audio transport control
    virtual int32_t StartPlayout() { return 0; }
    virtual int32_t StopPlayout() { return 0; }
    virtual bool Playing() const { return false; }
    virtual int32_t StartRecording() { return 0; }
    virtual int32_t StopRecording() { return 0; }
    virtual bool Recording() const { return false; }

    // Native sample rate controls (samples/sec)
    virtual int32_t SetRecordingSampleRate(const uint32_t samplesPerSec) {
        return 0;
    }
    virtual int32_t RecordingSampleRate(uint32_t* samplesPerSec) const {
        return 0;
    }
    virtual int32_t SetPlayoutSampleRate(const uint32_t samplesPerSec) {
        return 0;
    }
    virtual int32_t PlayoutSampleRate(uint32_t* samplesPerSec) const { return 0; }
 
    // Delay information and control
    virtual int32_t SetPlayoutBuffer(const BufferType type,
            uint16_t sizeMS = 0) {
        return 0;
    }
    virtual int32_t PlayoutBuffer(BufferType* type, uint16_t* sizeMS) const {
        return 0;
    }
    virtual int32_t PlayoutDelay(uint16_t* delayMS) const { return 0; }
    virtual int32_t RecordingDelay(uint16_t* delayMS) const { return 0; }

    // Audio mixer initialization
    virtual int32_t SpeakerIsAvailable(bool* available) {
        *available = true;
        return 0;
    }
    virtual int32_t InitSpeaker() { return 0; }
    virtual bool SpeakerIsInitialized() const { return true; }
    virtual int32_t MicrophoneIsAvailable(bool* available) {
        *available = true;
        return 0;
    }
    virtual int32_t InitMicrophone() { return 0; }
    virtual bool MicrophoneIsInitialized() const { return true; }

    // Speaker volume controls
    virtual int32_t SpeakerVolumeIsAvailable(bool* available) { 
        *available = true;
        return 0; 
    }
    virtual int32_t SetSpeakerVolume(uint32_t volume) { return 0; }
    virtual int32_t SpeakerVolume(uint32_t* volume) const { 
        *volume = 16; 
        return -1;
    }
    virtual int32_t MaxSpeakerVolume(uint32_t* maxVolume) const { 
        *maxVolume = 32;
        return 0; 
    }
    virtual int32_t MinSpeakerVolume(uint32_t* minVolume) const { 
        *minVolume = 0;
        return 0; 
    }
    virtual int32_t SpeakerVolumeStepSize(uint16_t* stepSize) const { 
        *stepSize = 1; 
        return 0; 
    }

    // Microphone volume controls
    virtual int32_t MicrophoneVolumeIsAvailable(bool* available) { 
        *available = true;
        return 0; 
    }
    virtual int32_t SetMicrophoneVolume(uint32_t volume) { return -1; }
    virtual int32_t MicrophoneVolume(uint32_t* volume) const { return -1; }
    virtual int32_t MaxMicrophoneVolume(uint32_t* maxVolume) const { return -1; }
    virtual int32_t MinMicrophoneVolume(uint32_t* minVolume) const { return -1; }
    virtual int32_t MicrophoneVolumeStepSize(uint16_t* stepSize) const { return -1; }

    // Speaker mute control
    virtual int32_t SpeakerMuteIsAvailable(bool* available) { 
        *available = false;
        return 0;
    }
    virtual int32_t SetSpeakerMute(bool enable) { return -1; }
    virtual int32_t SpeakerMute(bool* enabled) const { return -1; }

    // Microphone mute control
    virtual int32_t MicrophoneMuteIsAvailable(bool* available) { 
        *available = false;
        return 0;
    }
    virtual int32_t SetMicrophoneMute(bool enable) { return -1; }
    virtual int32_t MicrophoneMute(bool* enabled) const { return -1; }
    
    // Microphone boost control
    virtual int32_t MicrophoneBoostIsAvailable(bool* available) {
        *available = false;
        return 0; 
    }
    virtual int32_t SetMicrophoneBoost(bool enable) { return -1; }
    virtual int32_t MicrophoneBoost(bool* enabled) const { return -1; }
    
    // Stereo support
    virtual int32_t StereoPlayoutIsAvailable(bool* available) const {
        *available = false;
        return 0;
    }
    virtual int32_t SetStereoPlayout(bool enable) { 
        if (enable)
            return -1;
        return 0;
    }
    virtual int32_t StereoPlayout(bool* enabled) const {
        *enabled = false;
        return 0;
    }
    virtual int32_t StereoRecordingIsAvailable(bool* available) const {
        *available = false;
        return 0;
    }
    virtual int32_t SetStereoRecording(bool enable) {
        if (enable) {
            return -1;
        }
        return 0;
    }
    virtual int32_t StereoRecording(bool* enabled) const {
        *enabled = false;
        return 0;
    }
    // TODO
    virtual int32_t SetRecordingChannel(const ChannelType channel) { return 0; }
    virtual int32_t RecordingChannel(ChannelType* channel) const { return 0; }

    // Volume control based on the Windows Wave API (Windows only)
    virtual int32_t SetWaveOutVolume(uint16_t volumeLeft,
                           uint16_t volumeRight) { return 0; }
    virtual int32_t WaveOutVolume(uint16_t* volumeLeft,
                        uint16_t* volumeRight) const { return 0; }
    
    // Microphone Automatic Gain Control (AGC)
    virtual int32_t SetAGC(bool enable) { return 0; }
    virtual bool AGC() const { return true; }

    // CPU load
    virtual int32_t CPULoad(uint16_t* load) const { return -1; }

    // Recording of raw PCM data
    virtual int32_t StartRawOutputFileRecording(
            const char pcmFileNameUTF8[kAdmMaxFileNameSize]) { return 0; }
    virtual int32_t StopRawOutputFileRecording() { return 0; }
    virtual int32_t StartRawInputFileRecording(
            const char pcmFileNameUTF8[kAdmMaxFileNameSize]) { return 0; }
    virtual int32_t StopRawInputFileRecording() { return 0; }

    // Mobile device specific functions
    virtual int32_t ResetAudioDevice() { return 0; }
    virtual int32_t SetLoudspeakerStatus(bool enable) { return -1; }
    virtual int32_t GetLoudspeakerStatus(bool* enabled) const { return -1; }

    // *Experimental - not recommended for use.*
    // Enables the Windows Core Audio built-in AEC. Fails on other platforms.
    //
    // Must be called before InitRecording(). When enabled:
    // 1. StartPlayout() must be called before StartRecording().
    // 2. StopRecording() should be called before StopPlayout().
    //    The reverse order may cause garbage audio to be rendered or the
    //    capture side to halt until StopRecording() is called.
    virtual int32_t EnableBuiltInAEC(bool enable) { return -1; }
    virtual bool BuiltInAECIsEnabled() const { return false; }

private:
    CriticalSectionWrapper&     critSect_;
    AudioDeviceObserver*        ptrCbAudioDeviceObserver_;
    AudioTransport*             ptrCbAudioTransport_;
   
};

}  // namespace webrtc

#endif
