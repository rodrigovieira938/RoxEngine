#include "alina/alina.hpp"
#include <RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp>

namespace RoxEngine {
    UniversalRenderingPipeline::UniversalRenderingPipeline(alina::Device device) {
        mDevice = device;
        mCmd = std::shared_ptr<alina::ICommandList>(mDevice->createCommandList());
        mCmd->begin();
    }
    void UniversalRenderingPipeline::DrawMesh(RoxEngine::Mesh& mesh) {
        if(mesh.GetIndices().size() == 0 || mesh.GetPosition().size() == 0) return;
        auto meshData = mesh.GetData();
        if(mesh.NeedChange()) {
            meshData.BakeGPUResources(mDevice);
            mesh.SetNeedCHange(false);
        }
        if(!meshData.position_vb || !meshData.indices_vb) {
            return;
        }
        std::vector<alina::BindVertexBuffer> bindVBs = {alina::BindVertexBuffer().setBuffer(meshData.position_vb).setStride(sizeof(glm::vec3))};
        if(meshData.uvs_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData.uvs_vb).setStride(sizeof(glm::vec2)));
        if(meshData.normals_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData.normals_vb).setStride(sizeof(glm::vec3)));

        auto pipeline = mDevice->createGraphicsPipeline(
        alina::GraphicsPipelineDesc()
            .setInputLayout(meshData.inputLayout)
        );
        mCmd->bindGraphicsPipeline(pipeline);
        mCmd->bindVertexBuffers(bindVBs);
        mCmd->bindIndexBuffer(meshData.indices_vb);
        mCmd->drawIndexed(alina::DrawArguments().setVertexCount(mesh.GetIndices().size()));
    }
    void UniversalRenderingPipeline::Render() {
        mCmd->end();
        mDevice->execute(mCmd);
        mCmd->begin();
    }
}