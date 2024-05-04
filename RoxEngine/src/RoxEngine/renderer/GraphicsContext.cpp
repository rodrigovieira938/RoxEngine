#include "GraphicsContext.hpp"
#include "RoxEngine/platforms/GL/GLGraphicsContext.hpp"
#include "RoxEngine/profiler/profiler.hpp"

namespace RoxEngine{
    GraphicsContext* sContext = nullptr;


    void GraphicsContext::Init(RendererApi api) {
        PROFILER_FUNCTION();
        //TODO: add assertion if already exists
        sContext = new GL::GraphicsContext();
    }
    void GraphicsContext::ClearColor(float r,float g,float b,float a) {
        PROFILER_FUNCTION();
        sContext->VClearColor(r,g,b,a);
    }
    void GraphicsContext::ClearScreen(ClearScreenMask bufferBits) {
        PROFILER_FUNCTION();
        sContext->VClearScreen(bufferBits);
    }
    void GraphicsContext::Shutdown() {
        PROFILER_FUNCTION();
        delete sContext;
    }
}