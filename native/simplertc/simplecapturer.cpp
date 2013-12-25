#include <string>
#include <vector>

#include "simplecapturer.h"

class CapturerThread : public talk_base::MessageHandler {
public:
    CapturerThread(SimpleCapturer* capturer) : capturer_(capturer) {
        isRunning_ = false;
        thread_ = new talk_base::Thread();
        thread_->Start();
    }

    ~CapturerThread() {
        thread_->Quit();
        delete thread_;
    }

    void Start() {
        thread_->PostDelayed(100, this, MSG_CAPTURE_TIMER);
        isRunning_ = true;
    }
    
    void Stop() {
        thread_->Clear(this);
        isRunning_ = false; 
    }

    bool IsRunning() {
        return isRunning_;
    }

protected:    
    enum {
        MSG_CAPTURE_TIMER, 
    };
    
    void OnMessage(talk_base::Message* msg) {
        switch(msg->message_id) {
             
        }
    }
    



private:
    talk_base::Thread* thread_;
    bool isRunning_;
    SimpleCapturer* capturer_;
};

SimpleCapturer::SimpleCapturer() {
    cricket::VideoFormat format(640, 480, 
            FPS_TO_INTERVAL(15), 
            cricket::FOURCC_I420); 
    std::vector<cricket::VideoFormat> supported;
    supported.push_back(format);

    SetId("Simple");
    SetSupportedFormats(supported);

    capturerThread_ = new CapturerThread(this);
}

SimpleCapturer::~SimpleCapturer() {

}

cricket::CaptureState SimpleCapturer::Start(const cricket::VideoFormat& capture_format) {
    capturerThread_->Start();    
}

void SimpleCapturer::Stop() {
    capturerThread_->Stop();            
}

bool SimpleCapturer::IsRunning() {
    return capturerThread_->IsRunning();
}

bool SimpleCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs) {
    return true;
}

