#include <iostream>
#include "simplerenderer.h"

SimpleVideoRenderer::SimpleVideoRenderer() {
    std::cout << "Created a SimpleVideoRenderer" << std::endl;
}

SimpleVideoRenderer::~SimpleVideoRenderer() {

}

void SimpleVideoRenderer::SetSize(int width, int height) {
    std::cout << "SetSize width=" << width << " height=" << height;
}

void SimpleVideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {
    std::cout << " >>>>> RenderFrame >>>>> " << std::endl;
}


