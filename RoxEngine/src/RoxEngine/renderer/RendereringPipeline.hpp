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
        virtual void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, const Transform& transform = Transform()) = 0;
        virtual void Render() = 0;
    };
}