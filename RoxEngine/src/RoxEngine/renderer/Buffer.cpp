#include <cassert>
#include <memory>
#include "RoxEngine/profiler/profiler.hpp"
#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <RoxEngine/renderer/Buffer.hpp>
#include <RoxEngine/platforms/GL/GLBuffer.hpp>

namespace RoxEngine {
	uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        default: assert(false && "Unknown ShaderDataType!");
        }
        return 0;
	}

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
    std::shared_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height, const std::vector<FramebufferColorTexFormat>& attachments, FramebufferDepthTexFormat depthFormat) {
        PROFILER_FUNCTION();
        switch(GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::Framebuffer>(width,height,attachments,depthFormat);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
    }
}