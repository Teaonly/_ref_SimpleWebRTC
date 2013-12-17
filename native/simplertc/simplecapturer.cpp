#include <string>
#include <vector>


#include "simplecapturer.h"

SimpleCapturer::SimpleCapturer() {

}

SimpleCapturer::~SimpleCapturer() {

}

cricket::CaptureState SimpleCapturer::Start(const cricket::VideoFormat& capture_format) {

}

void SimpleCapturer::Stop() {
    
}

bool SimpleCapturer::IsRunning() {
    return false;
}

bool SimpleCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs) {
    return true;
}

