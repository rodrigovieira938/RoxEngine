#include "RoxEngine/profiler/profiler.hpp"
#include "glad/gl.h"
#include "GLGraphicsContext.hpp"
#include "GLFW/glfw3.h"
#include "RoxEngine/renderer/GraphicsContext.hpp"

namespace RoxEngine::GL {
    GraphicsContext::GraphicsContext() {
        PROFILER_FUNCTION();
        gladLoadGL(glfwGetProcAddress);
    }
    GraphicsContext::~GraphicsContext() {
        PROFILER_FUNCTION();
    }
    void GraphicsContext::VClearColor(float r, float g, float b, float a) {
        PROFILER_FUNCTION();
        glClearColor(r,g,b,a);
    }
    void GraphicsContext::VClearScreen(ClearScreenMask bufferBits) {
        PROFILER_FUNCTION();
        int mask = 0;
        if(bufferBits & ClearScreen::COLOR)
            mask |= GL_COLOR_BUFFER_BIT;
        if (bufferBits & ClearScreen::DEPTH) 
            mask |= GL_DEPTH_BUFFER_BIT;
        if (bufferBits & ClearScreen::STENCIL) 
            mask |= GL_STENCIL_BUFFER_BIT;
        glClear(mask);
    }
}