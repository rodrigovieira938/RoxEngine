#pragma once
#include <RoxEngine/renderer/Material.hpp>
#include <RoxEngine/renderer/Mesh.hpp>
#include <RoxEngine/renderer/Transform.hpp>

namespace RoxEngine {
    class RenderingPipeline
    {
    public:
        virtual ~RenderingPipeline() = default;
        //TODO: add transform
        virtual void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, const glm::mat4& transform = glm::mat4(1.0f)) = 0;
        void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, const Transform& transform = Transform()) {DrawMesh(mesh, material, transform.GetMatrix());}
        virtual void Render() = 0;
    };
}