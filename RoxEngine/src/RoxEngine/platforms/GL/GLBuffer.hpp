#include <RoxEngine/renderer/Buffer.hpp>

namespace RoxEngine::GL {
    class VertexBuffer final : public ::RoxEngine::VertexBuffer {
    public:
        VertexBuffer(void* vertices, size_t size);
        virtual ~VertexBuffer() final;
        virtual void SetData(const void* data, size_t size) override final;
        virtual const BufferLayout& GetLayout() const override final;
        virtual void SetLayout(const BufferLayout& layout) override final;

        uint32_t mID;
        size_t mSize;
        BufferLayout mLayout;
    };
    class IndexBuffer final : public ::RoxEngine::IndexBuffer {
    public:
        IndexBuffer(uint32_t* vertices, size_t count);
        virtual ~IndexBuffer() final;
        virtual void SetData(const uint32_t* data, size_t count) override final;
        virtual size_t GetCount() const final;

        uint32_t mID;
        size_t mCount;
        BufferLayout mLayout;
    };
    class Framebuffer final : public ::RoxEngine::Framebuffer {
    public:
        Framebuffer(uint32_t width, uint32_t height, const std::vector<FramebufferColorTexFormat>& attachments, FramebufferDepthTexFormat depthFormat);
        virtual ~Framebuffer() final;
        virtual std::pair<int,int> GetSize() final;

        uint32_t mID;
        int mWidth, mHeight;
        std::vector<uint32_t> mTextureIDS;
        std::vector<FramebufferColorTexFormat> mTextureFormats;
        std::pair<uint32_t,FramebufferColorTexFormat> mDepthTexture;
    };
}