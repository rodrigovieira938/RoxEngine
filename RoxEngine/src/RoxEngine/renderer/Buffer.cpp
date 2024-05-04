#include "Buffer.hpp"
#include <memory>

namespace RoxEngine {
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(size_t size) {
        return nullptr;
    }
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* vertices, size_t size) {
        return nullptr;
    }
    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, size_t count) {
        return nullptr;
    }
}