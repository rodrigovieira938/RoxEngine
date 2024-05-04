#include <RoxEngine/renderer/Buffer.hpp>
#include <RoxEngine/renderer/VertexArray.hpp>
#include <memory>

namespace RoxEngine::GL {
    class VertexArray final : public ::RoxEngine::VertexArray {
    public:
        VertexArray();
        virtual ~VertexArray() final;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override final;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override final;

        virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const final;
        virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const final;

        uint32_t mID = 0, mVBIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> mVBs;
        std::shared_ptr<IndexBuffer> mIB;
    };
}