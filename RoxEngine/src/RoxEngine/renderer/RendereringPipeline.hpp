#pragma once
#include <RoxEngine/renderer/Mesh.hpp>

namespace RoxEngine {
    class RenderingPipeline
    {
    public:
        virtual ~RenderingPipeline() = default;
        //TODO: add transform
        virtual void DrawMesh(RoxEngine::Mesh& mesh) = 0;
        virtual void Render() = 0;
    };
}