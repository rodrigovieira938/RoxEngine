#pragma once
#include <memory>
#include <RoxEngine/renderer/VertexArray.hpp>

namespace RoxEngine::GL {
    class VertexArray final : public ::RoxEngine::VertexArray {
    public:
        VertexArray();
        virtual ~VertexArray() final;

        virtual void AddVertexBuffer(const Ref<::RoxEngine::VertexBuffer>& vertexBuffer) override final;
        virtual void SetIndexBuffer(const Ref<::RoxEngine::IndexBuffer>& indexBuffer) override final;

        virtual const std::vector<Ref<::RoxEngine::VertexBuffer>>& GetVertexBuffers() const final;
        virtual const Ref<::RoxEngine::IndexBuffer>& GetIndexBuffer() const final;

        uint32_t mID = 0, mVBIndex = 0;
        std::vector<Ref<VertexBuffer>> mVBs;
        Ref<IndexBuffer> mIB;
    };
}