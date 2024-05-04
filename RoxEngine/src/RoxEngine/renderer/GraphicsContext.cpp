#include "GraphicsContext.hpp"
#include "RoxEngine/platforms/GL/GLGraphicsContext.hpp"

namespace RoxEngine{
    GraphicsContext* sContext = nullptr;


    void GraphicsContext::Init(RendererApi api) {
        //TODO: add assertion if already exists
        sContext = new GL::GraphicsContext();
    }
    void GraphicsContext::ClearColor(float r,float g,float b,float a) {
        sContext->VClearColor(r,g,b,a);
    }
    void GraphicsContext::ClearScreen(ClearScreenMask bufferBits) {
        sContext->VClearScreen(bufferBits);
    }
    void GraphicsContext::Shutdown() {
        delete sContext;
    }
}