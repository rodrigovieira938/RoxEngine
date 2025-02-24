#pragma once
#include <RoxEngine/renderer/RendererPipeline.hpp>
#include <RoxEngine/renderer/DefaultRendererPipeline/Rendeder.hpp>

namespace RoxEngine {
    struct DefaultRendererPipeline : public IRendererPipeline {
        void DrawMesh(Mesh& mesh, Material* material, const Transform& transform = Transform());
        void SetCamera(glm::mat4 viewProj);

        void Render();

        DefaultRenderer mRenderer;
    };
}