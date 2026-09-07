#pragma once
#include <RoxEngine/renderer/Material.hpp>
#include <RoxEngine/renderer/Mesh.hpp>
#include <RoxEngine/renderer/Transform.hpp>
#include <optional>

namespace RoxEngine {
    class RenderingPipeline
    {
    public:
        virtual ~RenderingPipeline() = default;
        //TODO: add transform
        virtual void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, std::optional<glm::mat4> transform = std::nullopt) = 0;
        void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, const Transform& transform = Transform()) {DrawMesh(mesh, material, transform.GetMatrix());}
        virtual void Render() = 0;
        virtual alina::Buffer GetSharedUbo(const ReflectedResource& resource) = 0;
    };
}