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
        case MSG_CAPTURE_TIMER:
            thread_->PostDelayed(66, this, MSG_CAPTURE_TIMER);
            capturer_->onCaptureTimer(time_stamp_);
            time_stamp_ += 66666666;  // 15 fps
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
    cricket::VideoFormat format(704, 576, 
            FPS_TO_INTERVAL(15), 
            cricket::FOURCC_I420); 
    myFormat_ = format;
    supported.push_back(myFormat_);

    yuvBuffer_ = (unsigned char *)malloc(704 * 576 * 2);

    SetId("SimpleVideo");
    SetSupportedFormats(supported);
    capturerThread_ = new CapturerThread(this);
}

SimpleCapturer::~SimpleCapturer() {
    free(yuvBuffer_);
}

cricket::CaptureState SimpleCapturer::Start(const cricket::VideoFormat& capture_format) {
    std::cout << ">>>>>>>>>>>> Begin Start Capture....." << std::endl;

    capturerThread_->Start();    
    return cricket::CS_RUNNING;
}

void SimpleCapturer::Stop() {
    std::cout << "Called with Stop " << std::endl;
    capturerThread_->Stop();  
    SetCaptureFormat(NULL);    
}

bool SimpleCapturer::IsRunning() {
    return capturerThread_->IsRunning();
}

bool SimpleCapturer::GetPreferredFourccs(std::vector<uint32>* fourccs) {
    std::cout << ">>>>>>>>>>>> Called with GetPreferredFourccs " << std::endl;
    
    if (!fourccs) {
        return false;
    }
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
    frame.data = yuvBuffer_;
    
    {
        static FILE* fp = NULL;
        if (fp == NULL) {
            fp = fopen("./4cif.yuv", "rb");
        }
        if ( feof(fp)) {
            fseek(fp, 0l, SEEK_SET);    
        }
        fread(yuvBuffer_, 704*576*1.5, 1, fp);
    }
     
    SignalFrameCaptured(this, &frame); 
}


