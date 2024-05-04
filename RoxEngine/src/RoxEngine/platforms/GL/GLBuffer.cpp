#include <glad/gl.h>
#include "GLBuffer.hpp"
#include "RoxEngine/renderer/Buffer.hpp"

namespace RoxEngine::GL {
    VertexBuffer::VertexBuffer(void* vertices, size_t size) : mSize(size) {
        glCreateBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    }
    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &mID);
    }
    void VertexBuffer::SetData(const void* data, size_t size) {
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        if(size > mSize || size < mSize) {
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
            mSize = size;
        }
        else {
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        }
    }
    const BufferLayout& VertexBuffer::GetLayout() const {
        return mLayout;
    }
    void VertexBuffer::SetLayout(const BufferLayout& layout) {
        mLayout = layout;
    }

    IndexBuffer::IndexBuffer(uint32_t* data,size_t count) {
        glCreateBuffers(1, &mID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
        mCount = count;
    }
    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &mID);
    }
    void IndexBuffer::SetData(const uint32_t *data, size_t count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
        if(count > mCount || count < mCount) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
            mCount = count;
        }
        else {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count*sizeof(uint32_t), data);
        }
    }
    size_t IndexBuffer::GetCount() const {
        return mCount;
    }
}