#include "RoxEngine/profiler/profiler.hpp"
#include <RoxEngine/renderer/GraphicsContext.hpp>
#include <RoxEngine/platforms/GL/GLVertexArray.hpp>

namespace RoxEngine {
    Ref<VertexArray> VertexArray::Create() {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::VertexArray>();
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
}