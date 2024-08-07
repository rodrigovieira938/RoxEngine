module;
#include <glad/gl.h>
module roxengine;

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
    uint32_t FramebufferColorTexFormatToGl(FramebufferColorTexFormat format) {
        switch (format) {
            case RoxEngine::FramebufferColorTexFormat::R8:
                return GL_R8;
            case RoxEngine::FramebufferColorTexFormat::RG8:
                return GL_RG8;
            case RoxEngine::FramebufferColorTexFormat::RGB8:
                return GL_RGB8;
            case RoxEngine::FramebufferColorTexFormat::RGBA8:
                return GL_RGBA8;
            case RoxEngine::FramebufferColorTexFormat::R16:
                return GL_R16;
            case RoxEngine::FramebufferColorTexFormat::RG16:
                return GL_RG16;
            case RoxEngine::FramebufferColorTexFormat::RGB16:
                return GL_RGB16;
            case RoxEngine::FramebufferColorTexFormat::RGBA16:
                return GL_RGBA16;
            case RoxEngine::FramebufferColorTexFormat::R32:
                return GL_R32F;
            case RoxEngine::FramebufferColorTexFormat::RG32:
                return GL_RG32F;
            case RoxEngine::FramebufferColorTexFormat::RGB32:
                return GL_RGB32F;
            case RoxEngine::FramebufferColorTexFormat::RGBA32:
                return GL_RGBA32F;
        }
    }
    uint32_t FramebufferDepthTexFormatToGl(FramebufferDepthTexFormat format) {
        switch (format) {
            case RoxEngine::FramebufferDepthTexFormat::D32F:
            case RoxEngine::FramebufferDepthTexFormat::D32FS8U: return GL_DEPTH32F_STENCIL8;
            case RoxEngine::FramebufferDepthTexFormat::D24UNS8U: return GL_DEPTH24_STENCIL8;
        }
    }
    Framebuffer::Framebuffer(uint32_t width, uint32_t height, const std::vector<FramebufferColorTexFormat>& attachments, FramebufferDepthTexFormat depthFormat) {
        glGenFramebuffers(1, &mID);
        glBindFramebuffer(GL_FRAMEBUFFER, mID);
        mWidth = width;
        mHeight = height;

        mTextureIDS.resize(attachments.size());
        mTextureIDS.resize(attachments.size());
        glGenTextures(mTextureIDS.size(),mTextureIDS.data());
        for(int i = 0; i < mTextureIDS.size(); i++) {
            auto texture = mTextureIDS[i];
            auto format = attachments[i];
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, FramebufferColorTexFormatToGl(format), width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, texture, 0);
        }
        //TODO: use an rbo instead of a plain texture
        auto format = depthFormat;
        glGenTextures(1, &mDepthTexture.first);
        glBindTexture(GL_TEXTURE_2D, mDepthTexture.first);
        glTexImage2D(GL_TEXTURE_2D, 0, FramebufferDepthTexFormatToGl(format), width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthTexture.first, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &mID);
    }
    std::pair<int,int> Framebuffer::GetSize() {
        return {mWidth,mHeight};
    }
}