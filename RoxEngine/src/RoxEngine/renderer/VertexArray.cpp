#include "VertexArray.hpp"
#include "RoxEngine/profiler/profiler.hpp"
#include <RoxEngine/platforms/GL/GLVertexArray.hpp>
#include <RoxEngine/renderer/GraphicsContext.hpp>

namespace RoxEngine {
    std::shared_ptr<VertexArray> VertexArray::Create() {
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