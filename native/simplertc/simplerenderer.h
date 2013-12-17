#ifndef _SIMPLERENDERER_H_
#define _SIMPLERENDERER_H_

#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/media/base/mediachannel.h"
#include "talk/media/base/videocommon.h"
#include "talk/media/base/videoframe.h"
#include "talk/media/base/videorenderer.h"

class SimpleVideoRenderer : public webrtc::VideoRendererInterface {
public:
    SimpleVideoRenderer();
    virtual ~SimpleVideoRenderer();
    
    // VideoRendererInterface implementation
    virtual void SetSize(int width, int height);
    virtual void RenderFrame(const cricket::VideoFrame* frame);
};

#endif
