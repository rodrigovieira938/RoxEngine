#include <RoxEngine/renderer/DefaultRendererPipeline/DefaultRendererPipeline.hpp>

namespace RoxEngine {
    void DefaultRendererPipeline::DrawMesh(Mesh& mesh, Material* material, const Transform& transform) {
        mRenderer.DrawMesh(mesh);
    }
    void DefaultRendererPipeline::SetCamera(glm::mat4 viewProj) {
        mRenderer.SetCamera(viewProj, glm::mat4(1.0f));
    }
    void DefaultRendererPipeline::Render() {
        mRenderer.Render();
    }
}