#ifndef _SIMPLECAPTURER_H_
#define _SIMPLECAPTURER_H_

#include <string>
#include <vector>

#include "talk/base/stream.h"
#include "talk/base/stringutils.h"
#include "talk/media/base/videocapturer.h"

class CapturerThread;
// Simulated video capturer that periodically reads frames from a file.
class SimpleCapturer : public cricket::VideoCapturer {
public:
    SimpleCapturer();
    virtual ~SimpleCapturer();

    void onCaptureTimer(int64 ts);

    // Override virtual methods of parent class VideoCapturer.
    virtual cricket::CaptureState Start(const cricket::VideoFormat& capture_format);
    virtual void Stop();
    virtual bool IsRunning();
    virtual bool IsScreencast() const { return false; }
    virtual bool GetPreferredFourccs(std::vector<uint32>* fourccs);

private:
    unsigned char *yuvBuffer_;
    cricket::VideoFormat myFormat_;     
    CapturerThread* capturerThread_;
    DISALLOW_COPY_AND_ASSIGN(SimpleCapturer);
};

#endif
