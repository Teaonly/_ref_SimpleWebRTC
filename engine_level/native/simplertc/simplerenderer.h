#ifndef _SIMPLERENDERER_H_
#define _SIMPLERENDERER_H_

#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/media/base/mediachannel.h"
#include "talk/media/base/videocommon.h"
#include "talk/media/base/videoframe.h"
#include "talk/media/base/videorenderer.h"

typedef struct _GtkWidget GtkWidget;
typedef union _GdkEvent GdkEvent;

class SimpleVideoRenderer : public webrtc::VideoRendererInterface, public talk_base::MessageHandler {
public:
    SimpleVideoRenderer();
    virtual ~SimpleVideoRenderer();
    
    void OnRedraw();

    // MessageHandler
    virtual void OnMessage(talk_base::Message *msg);

    // VideoRendererInterface implementation
    virtual void SetSize(int width, int height);
    virtual void RenderFrame(const cricket::VideoFrame* frame);

private:
    GtkWidget* window_;  // Our main window.
    GtkWidget* draw_area_;  // The drawing surface for rendering video streams. 

    talk_base::Thread* gui_thread_;    
    int width_;
    int height_;
    talk_base::scoped_array<uint8> image_;
};


#endif
