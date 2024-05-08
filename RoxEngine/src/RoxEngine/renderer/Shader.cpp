#include "Shader.hpp"
#include "RoxEngine/profiler/profiler.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include "RoxEngine/platforms/GL/GLShader.hpp"

namespace RoxEngine {
    Ref<Shader> Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc) {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::Shader>(vertexSrc, fragmentSrc);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
}