#include "alina/alina.hpp"
#include <RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp>

namespace RoxEngine {
    UniversalRenderingPipeline::UniversalRenderingPipeline(Ref<alina::IDevice> device) {
        mDevice = device;
        mCmd = std::shared_ptr<alina::ICommandList>(mDevice->createCommandList());
        mCmd->begin();
    }
    void UniversalRenderingPipeline::DrawMesh(RoxEngine::Mesh& mesh) {
        if(mesh.GetIndices().size() == 0) return;
        auto vb = 
            mDevice->createBuffer(alina::BufferDesc()
                    .setDebugName("VertexBuffer - UniversalRenderingPipeline::DrawMesh")
                    .setType(alina::BufferType::VERTEX));
        auto ib =
            mDevice->createBuffer(
                alina::BufferDesc()
                    .setDebugName("VertexBuffer - UniversalRenderingPipeline::DrawMesh")
                    .setType(alina::BufferType::INDEX));
        //TODO: add support for uvs and normals
        auto cmd = mDevice->createCommandList();
        cmd->begin();
        cmd->writeBuffer(vb, mesh.GetPosition().data(), mesh.GetPosition().size() * sizeof(glm::vec3), 0);
        cmd->writeBuffer(ib, mesh.GetIndices().data(), mesh.GetIndices().size() * sizeof(uint32_t), 0);
        cmd->end();
        mDevice->execute(cmd);
        auto pipeline = mDevice->createGraphicsPipeline(
        alina::GraphicsPipelineDesc()
            .setInputLayout(
                mDevice->createInputLayout({
                    alina::VertexAttributeDesc().setFormat(alina::VertexAttributeFormat::Float).setArraySize(3).setStride(sizeof(glm::vec3)),
                })
            )
        );
        mCmd->bindGraphicsPipeline(pipeline);
        mCmd->bindVertexBuffers({
            alina::BindVertexBuffer().setBuffer(vb).setStride(sizeof(glm::vec3)),
        });
        mCmd->bindIndexBuffer(ib);
        mCmd->drawIndexed(alina::DrawArguments().setVertexCount(mesh.GetIndices().size()));
    }
    void UniversalRenderingPipeline::Render() {
        mCmd->end();
        mDevice->execute(mCmd.get());
        mCmd->begin();
    }
}