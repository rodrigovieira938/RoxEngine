#include "Buffer.hpp"
#include "RoxEngine/profiler/profiler.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <RoxEngine/platforms/GL/GLBuffer.hpp>

namespace RoxEngine {
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(size_t size) {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::VertexBuffer>(nullptr, size);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, size_t size) {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::VertexBuffer>(vertices,size);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, size_t count) {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::IndexBuffer>(indices, count);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
}