#include "RoxEngine/renderer/Material.hpp"
#include "RoxEngine/renderer/Transform.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp>
#include <RoxEngine/core/Logger.hpp>
#include <alina/opengl.hpp>
#include <cstring>

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
        //TODO: replace this with a proper default framebuffer reference when alina supports it
        mOutputFb = ((alina::opengl::IGlDevice*)device.get())->createUnmanagedFramebuffer(0);
    }
    void UniversalRenderingPipeline::DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, std::optional<glm::mat4> transform) {
        if(mesh.GetIndices().size() == 0 || mesh.GetPosition().size() == 0) return;
        auto meshData = mesh.GetData();
        if(mesh.NeedChange()) {
            meshData->BakeGPUResources(mDevice, &mInputLayoutPool);
            mesh.SetNeedChange(false);
        }
        if(!meshData->position_vb || !meshData->indices_vb) {
            return;
        }
        std::vector<alina::BindVertexBuffer> bindVBs = {alina::BindVertexBuffer().setBuffer(meshData->position_vb).setStride(sizeof(glm::vec3))};
        //TODO: add way to specify which vertex buffers to use for a mesh, since not all meshes will have the same vertex buffer layout. For now we will just check for the most common ones and use them if they exist.
        //if(meshData->uvs_vb)
        //    bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData->uvs_vb).setStride(sizeof(glm::vec2)));
        //if(meshData->normals_vb)
        //    bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData->normals_vb).setStride(sizeof(glm::vec3)));
        

        mCmd->bindGraphicsPipeline(material.GetGraphicsPipeline(mInputLayoutPool, mGraphicsPipelinePool));
        auto shaderResources = material.GetShaderResources();
        if(transform.has_value()){
            //TODO: 
            log::warn(__FILE__":{}  TODO: implement renderer updating transform", __LINE__);
        }
        mCmd->bindShaderResources(shaderResources);
        mCmd->bindVertexBuffers(bindVBs);
        mCmd->bindIndexBuffer(meshData->indices_vb);
        mCmd->drawIndexed(alina::DrawArguments().setVertexCount(mesh.GetIndices().size()));
    }
    void UniversalRenderingPipeline::Render() {
        mCmd->endRenderPass();
        mCmd->end();
        mDevice->execute(mCmd);
    }
    void UniversalRenderingPipeline::Begin(glm::mat4 viewMatrix, glm::mat4 projMatrix) {
        mGlobals.viewMatrix = viewMatrix;
        mGlobals.projMatrix = projMatrix;
        if(mGlobalsUboReflection.has_value()) {
            glm::mat4 viewProj = mGlobals.projMatrix * mGlobals.viewMatrix;
            auto set = [&](const std::string_view& name, const auto& value){
                for(auto& field : mGlobalsUboReflection->layout->fields) {
                    if(field.name == name) {
                        if(auto scalar = std::get_if<ScalarInfo>(&field.data)) {
                            mCmd->writeBuffer(mGlobalsUbo, &value, sizeof(value), field.offset);
                            return;
                        }
                    }
                }
            };
            //TODO: lets assume the reflection is correct and the fields exist, but we should check for that and log an error if they don't
            mCmd->begin();
            set("viewMatrix", mGlobals.viewMatrix);
            set("projMatrix", mGlobals.projMatrix);
            set("viewProjMatrix", viewProj);
            set("cameraPosition[0]", mGlobals.camPos.x);
            set("cameraPosition[1]", mGlobals.camPos.y);
            set("cameraPosition[2]", mGlobals.camPos.z);
            set("cameraDirection[0]", mGlobals.camDir.x);
            set("cameraDirection[1]", mGlobals.camDir.y);
            set("cameraDirection[2]", mGlobals.camDir.z);
            mCmd->end();
            mDevice->execute(mCmd);
        }
        mCmd->begin();
        //Disabling this until alina support referencing the default framebuffer
        mCmd->beginRenderPass(alina::RenderPassDesc().setFramebuffer(mOutputFb).setAttachmentsLoadOp({alina::RenderPassLoadOp::CLEAR}).setAttachmentsClearColors({{0,0,0,0}}));
        mCmd->beginSubPass(alina::SubPassDesc().setAttachments({alina::SubPassAttachment::COLOR}));
    }
    alina::Buffer UniversalRenderingPipeline::GetSharedUbo(const ReflectedResource& resource) {
        if(mSharedUbos.find(resource.sharedName) != mSharedUbos.end()) {
            return mSharedUbos[resource.sharedName];
        }
        auto ubo = mDevice->createBuffer(alina::BufferDesc().setDebugName("URP - Shared Ubo: " + resource.sharedName).setType(alina::BufferType::UNIFORM));
        auto cmd = mDevice->createCommandList();
        cmd->begin();
        cmd->writeBuffer(ubo, nullptr, resource.layout->size, 0);
        cmd->end();
        mDevice->execute(cmd);
        mSharedUbos[resource.sharedName] = ubo;
        
        //TODO: embed the globals ubo reflection
        if(!mGlobalsUboReflection.has_value()) {
            if(resource.sharedName == "RoxEngine.URP.Globals") {
                mGlobalsUboReflection = resource;
                mGlobalsUbo = ubo;
            }    
        }
        
        return ubo;
    }
}