#include "alina/alina.hpp"
#include <RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp>

namespace RoxEngine {
    UniversalRenderingPipeline::UniversalRenderingPipeline(alina::Device device) : mInputLayoutPool(device), mGraphicsPipelinePool(device) {
        mDevice = device;
        mCmd = mDevice->createCommandList();
        mFbTex = device->createTexture(
            alina::TextureDesc().setDebugName("URP Framebuffer Texture").setWidth(800).setHeight(800)
        );
        mFb = device->createFramebuffer(
            alina::FramebufferDesc()
                .setColorAttachments({
                    alina::FramebufferAttachment().setTexture(mFbTex)
                })
                .setDebugName("URP Framebuffer")
        );
        begin();
    }
    void UniversalRenderingPipeline::DrawMesh(RoxEngine::Mesh& mesh) {
        if(mesh.GetIndices().size() == 0 || mesh.GetPosition().size() == 0) return;
        auto meshData = mesh.GetData();
        if(mesh.NeedChange()) {
            meshData.BakeGPUResources(mDevice, &mInputLayoutPool);
            mesh.SetNeedChange(false);
        }
        if(!meshData.position_vb || !meshData.indices_vb) {
            return;
        }
        std::vector<alina::BindVertexBuffer> bindVBs = {alina::BindVertexBuffer().setBuffer(meshData.position_vb).setStride(sizeof(glm::vec3))};
        if(meshData.uvs_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData.uvs_vb).setStride(sizeof(glm::vec2)));
        if(meshData.normals_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData.normals_vb).setStride(sizeof(glm::vec3)));
        auto pipeline = mGraphicsPipelinePool.Get(alina::GraphicsPipelineDesc().setInputLayout(meshData.inputLayout));
        mCmd->bindGraphicsPipeline(pipeline);
        mCmd->bindVertexBuffers(bindVBs);
        mCmd->bindIndexBuffer(meshData.indices_vb);
        mCmd->drawIndexed(alina::DrawArguments().setVertexCount(mesh.GetIndices().size()));
    }
    void UniversalRenderingPipeline::Render() {
        mCmd->endRenderPass();
        mCmd->end();
        mDevice->execute(mCmd);
        begin();
    }
    void UniversalRenderingPipeline::begin() {
        mCmd->begin();
        mCmd->beginRenderPass(alina::RenderPassDesc().setFramebuffer(mFb).setAttachmentsLoadOp({alina::RenderPassLoadOp::CLEAR}).setAttachmentsClearColors({{0,0,0,0}}));
        mCmd->beginSubPass(alina::SubPassDesc().setAttachments({alina::SubPassAttachment::COLOR}));
    }
}