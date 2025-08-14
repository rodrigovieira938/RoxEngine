#pragma once
#include <RoxEngine/renderer/Material.hpp>
#include <RoxEngine/renderer/Mesh.hpp>

namespace RoxEngine {
    class RenderingPipeline
    {
    public:
        virtual ~RenderingPipeline() = default;
        //TODO: add transform
        virtual void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material) = 0;
        virtual void Render() = 0;
    };
}