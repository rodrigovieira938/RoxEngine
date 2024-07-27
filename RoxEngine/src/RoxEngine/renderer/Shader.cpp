#include "Shader.hpp"
#include "RoxEngine/profiler/profiler.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include "RoxEngine/platforms/GL/GLShader.hpp"

namespace RoxEngine {
    Ref<Shader> Shader::Create(const std::string& src, const std::string& module_name)
    {
        PROFILER_FUNCTION();
        switch (GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::Shader>(src, module_name);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& path)
    {
        PROFILER_FUNCTION();
        switch (GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::Shader>(path);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
}
