#include "simpleaudiodevice.h"
#include "webrtc/system_wrappers/interface/critical_section_wrapper.h"                                                                                        
#include "webrtc/system_wrappers/interface/trace.h"

namespace webrtc {

enum { kAdmMaxIdleTimeProcess = 1000 };

SimpleAudioDevice::SimpleAudioDevice() :
        critSect_(*CriticalSectionWrapper::CreateCriticalSection()),
        ptrCbAudioDeviceObserver_(NULL) { 
    
}

SimpleAudioDevice::~SimpleAudioDevice() {
    
}

int32_t SimpleAudioDevice::TimeUntilNextProcess() {
    return kAdmMaxIdleTimeProcess;
}

int32_t SimpleAudioDevice::Process() {
    return 0;
}

int32_t SimpleAudioDevice::RegisterEventObserver(AudioDeviceObserver* eventCallback) {
    CriticalSectionScoped lock(&critSect_);
    ptrCbAudioDeviceObserver_ = eventCallback;
}

int32_t SimpleAudioDevice::RegisterAudioCallback(AudioTransport* audioCallback) {
    CriticalSectionScoped lock(&critSect_);
    ptrCbAudioTransport_ = audioCallback;
}

void SimpleAudioDevice::Test() {

}

}  // namespace webrtc
