#include "RoxEngine/profiler/gpu_profiler.hpp"
#include "RoxEngine/profiler/profiler.hpp"
#include "RoxEngine/renderer/VertexArray.hpp"
#include "glad/gl.h"
#include "GLGraphicsContext.hpp"
#include "GLFW/glfw3.h"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <RoxEngine/platforms/GL/GLVertexArray.hpp>
#include <RoxEngine/platforms/GL/GLBuffer.hpp>
#include <memory>
#include <RoxEngine/platforms/GL/GLShader.hpp>
namespace RoxEngine::GL {
    void GLAPIENTRY 
    MessageCallback( GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam )
    {
        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
    }
    GraphicsContext::GraphicsContext() {
        PROFILER_FUNCTION();
        gladLoadGL(glfwGetProcAddress);
        #ifdef ENABLE_PROFILER
        gladSetGLPreCallback(RoxEngine::Profiler::glad_prefunc);
        gladSetGLPostCallback(RoxEngine::Profiler::glad_posfunc);
        #endif
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
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
    void GraphicsContext::VUseShader(Ref<::RoxEngine::Shader> shader) {
        glUseProgram(std::static_pointer_cast<GL::Shader>(shader)->mID);
    }
    void GraphicsContext::VDraw(Ref<::RoxEngine::VertexArray> va, uint32_t indexCount) {
        auto glva = std::static_pointer_cast<VertexArray>(va);
        glBindVertexArray(glva->mID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::static_pointer_cast<IndexBuffer>(glva->mIB)->mID);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
}