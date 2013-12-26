#include <string>
#include <iostream>
#include <vector>

#include "simplecapturer.h"

class CapturerThread : public talk_base::MessageHandler {
public:
    CapturerThread(SimpleCapturer* capturer) : capturer_(capturer) {
        isRunning_ = false;
        thread_ = new talk_base::Thread();
        thread_->Start();
        
        time_stamp_ = 0; 
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
            capturer_->onCaptureTimer(time_stamp_);
            time_stamp_ += 33333333;  // 30 fps
        }
    }
    
private:
    talk_base::Thread* thread_;
    bool isRunning_;
    SimpleCapturer* capturer_;

    int64 time_stamp_;
};

SimpleCapturer::SimpleCapturer() {
    std::vector<cricket::VideoFormat> supported;
    supported.push_back(myFormat_);

    SetId("Simple");
    SetSupportedFormats(supported);
    capturerThread_ = new CapturerThread(this);

    cricket::VideoFormat format(640, 480, 
            FPS_TO_INTERVAL(15), 
            cricket::FOURCC_I420); 
    myFormat_ = format;
}

SimpleCapturer::~SimpleCapturer() {

}

cricket::CaptureState SimpleCapturer::Start(const cricket::VideoFormat& capture_format) {
    std::cout << "Begin Start Capture....." << std::endl;

    capturerThread_->Start();    
    return cricket::CS_RUNNING;
}

void SimpleCapturer::Stop() {
    capturerThread_->Stop();  
    SetCaptureFormat(NULL);    
}

bool SimpleCapturer::IsRunning() {
    return capturerThread_->IsRunning();
}

bool SimpleCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs) {
    if (!fourccs) {
        return false;
    }
    std::cout << "Called with GetPreferredFourccs " << std::endl;
    fourccs->push_back( cricket::FOURCC_I420 );  
    return true;  
}

void SimpleCapturer::onCaptureTimer(int64 ts) {
    cricket::CapturedFrame frame;

    frame.width = myFormat_.width;
    frame.height = myFormat_.height;
    frame.fourcc = myFormat_.fourcc;
    frame.data_size = (myFormat_.width * myFormat_.height) + (myFormat_.width * myFormat_.height) / 2;
    frame.elapsed_time = ts;
    frame.time_stamp = ts;

    // TODO data
    SignalFrameCaptured(this, &frame); 
}


