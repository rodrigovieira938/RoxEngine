#pragma once
#include <memory>
#include <vector>
#include <RoxEngine/utils/Utils.hpp>
#include <RoxEngine/renderer/Buffer.hpp>

namespace RoxEngine {
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
}