#include <iostream>
#include <stddef.h>
#include "simplerenderer.h"

SimpleVideoRenderer::SimpleVideoRenderer() {
}

SimpleVideoRenderer::~SimpleVideoRenderer() {

}

void SimpleVideoRenderer::SetSize(int width, int height) {
}

void SimpleVideoRenderer::RenderFrame(const cricket::VideoFrame* frame) {
    std::cout << "One Picture is Rendered!" << std::endl;
}

