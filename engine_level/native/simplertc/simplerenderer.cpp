#include <iostream>
#include "simplerenderer.h"
#include <gtk/gtk.h>
#include <stddef.h>

SimpleVideoRenderer::SimpleVideoRenderer() {
    gui_thread_ = new talk_base::Thread();
    gui_thread_->Start();
    gui_thread_->Post(this, 0);
}

SimpleVideoRenderer::~SimpleVideoRenderer() {

}

void SimpleVideoRenderer::OnMessage(talk_base::Message *msg) {
    gtk_init(NULL, NULL);
    g_type_init();
    g_thread_init(NULL);
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (window_) {
        gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(window_), 640, 480);
        gtk_window_set_title(GTK_WINDOW(window_), "RemoteVideo");
        
        gtk_container_set_border_width(GTK_CONTAINER(window_), 0);
        draw_area_ = gtk_drawing_area_new();
        gtk_container_add(GTK_CONTAINER(window_), draw_area_);

        gtk_widget_show_all(window_);
    }  
    gtk_main();
       
}

void SimpleVideoRenderer::SetSize(int width, int height) {
    gdk_threads_enter();
    width_ = width;
    height_ = height;
    image_.reset(new uint8[width * height * 4]);
    gdk_threads_leave();
}

gboolean Redraw(gpointer data) {
    SimpleVideoRenderer* wnd = reinterpret_cast<SimpleVideoRenderer*>(data);
    wnd->OnRedraw();
    return false;
}

void SimpleVideoRenderer::OnRedraw() {
    gdk_threads_enter();

    gdk_draw_rgb_32_image(draw_area_->window,
            draw_area_->style->fg_gc[GTK_STATE_NORMAL],
            0,
            0,
            width_,
            height_,
            GDK_RGB_DITHER_MAX,
            image_.get(),
            width_ * 4 );

    gdk_threads_leave();
}

void SimpleVideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {
    gdk_threads_enter();

    int size = width_ * height_ * 4;
    // TODO: Convert directly to RGBA
    frame->ConvertToRgbBuffer(cricket::FOURCC_ARGB,
            image_.get(),
            size,
            width_ * 4);
    // Convert the B,G,R,A frame to R,G,B,A, which is accepted by GTK.
    // The 'A' is just padding for GTK, so we can use it as temp.
    uint8* pix = image_.get();
    uint8* end = image_.get() + size;
    while (pix < end) {
        pix[3] = pix[0];     // Save B to A.
        pix[0] = pix[2];  // Set Red.
        pix[2] = pix[3];  // Set Blue.
        pix[3] = 0xFF;     // Fixed Alpha.
        pix += 4;
    }

    gdk_threads_leave();

    g_idle_add(Redraw, this); 
}


