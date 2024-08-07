module;
#include "RoxEngine/profiler/profiler.hpp"
module roxengine;

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