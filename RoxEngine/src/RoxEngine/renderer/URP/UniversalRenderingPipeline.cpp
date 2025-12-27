#include "RoxEngine/renderer/Material.hpp"
#include "RoxEngine/renderer/Transform.hpp"
#include "RoxEngine/slang/slang.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp>
#include <alina/opengl.hpp>
#include <cstring>

namespace RoxEngine {
    UniversalRenderingPipeline::UniversalRenderingPipeline(alina::Device device) : mInputLayoutPool(device), mGraphicsPipelinePool(device) {
        SlangLayer::Init();
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

        auto module = SlangLayer::CompileModule("res://shaders/internal/urp.slang");
        auto program = SlangLayer::LinkModule(module);
        mGlobalsUboReflection = SlangLayer::GetProgramReflection(program);
        mGlobalsUbo = mDevice->createBuffer(alina::BufferDesc().setDebugName("URP Globals UBO"));
        int i = 0;
        for(auto& ubo : mGlobalsUboReflection.shared_ubos) {
            if(strcmp(ubo.index_name.c_str(), "urp.SLANG_ParameterGroup_UrpGlobals") == 0) {
                mGlobalUboIndex = i;
            }
            i++;
        }
        mCmd->begin();
        mCmd->writeBuffer(mGlobalsUbo, nullptr, mGlobalsUboReflection.shared_ubos[mGlobalUboIndex].size, 0);
        mCmd->end();
        mDevice->execute(mCmd);
    }
    void UniversalRenderingPipeline::DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, const Transform& transform) {
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
        if(meshData->uvs_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData->uvs_vb).setStride(sizeof(glm::vec2)));
        if(meshData->normals_vb)
            bindVBs.push_back(alina::BindVertexBuffer().setBuffer(meshData->normals_vb).setStride(sizeof(glm::vec3)));
        auto pipeline_desc = alina::GraphicsPipelineDesc()
            .setInputLayout(meshData->inputLayout)
            .setVertexShader(material.GetVertexShader())
            .setFragmentShader(material.GetFragmentShader());
        auto pipeline = mGraphicsPipelinePool.Get(pipeline_desc);
        mCmd->bindGraphicsPipeline(pipeline);
        auto shaderResources = material.GetShaderResources();
        auto moduleReflection = material.GetModuleReflection();
        for(auto& reflection_ubo : moduleReflection->shared_ubos) {
            if(reflection_ubo.index_name == mGlobalsUboReflection.shared_ubos[mGlobalUboIndex].index_name) {
                for(auto& binding: shaderResources.uboBinding) {
                    if(binding.binding == reflection_ubo.binding_index && binding.set == reflection_ubo.binding_space) {
                        binding.setBuffer(mGlobalsUbo);
                        break;
                    }
                }
                break;
            }
        }
        {
            auto cmd = mDevice->createCommandList();
            cmd->begin();
            auto transform_matrix = transform.GetMatrix();
            auto lookup = mGlobalsUboReflection.lookup("transformMatrix");
            if(lookup) {
                cmd->writeBuffer(mGlobalsUbo, &transform_matrix[0][0], sizeof(glm::mat4), lookup->offset);
            }
            cmd->end();
            mDevice->execute(cmd); //Gotta execute now since writebuffer takes a ptr into the stack
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
        glm::mat4 viewProj = mGlobals.projMatrix * mGlobals.viewMatrix;
        mCmd->begin();
        static auto setMatrix4 = [&](const char * path, auto& value){
            auto lookup = mGlobalsUboReflection.lookup(path);
            if(lookup) {
                mCmd->writeBuffer(mGlobalsUbo, &value, sizeof(glm::mat4), lookup->offset);
            }
        };
        static auto setFloat= [&](const char * path, float& value){
            auto lookup = mGlobalsUboReflection.lookup(path);
            if(lookup) {
                mCmd->writeBuffer(mGlobalsUbo, &value, sizeof(float), lookup->offset);
            }
        };
        setMatrix4("viewMatrix", mGlobals.viewMatrix);
        setMatrix4("projMatrix", mGlobals.projMatrix);
        setMatrix4("viewProjMatrix", viewProj);
        setFloat("cameraPosition[0]", mGlobals.camPos.x);
        setFloat("cameraPosition[1]", mGlobals.camPos.y);
        setFloat("cameraPosition[2]", mGlobals.camPos.z);
        setFloat("cameraDirection[0]", mGlobals.camDir.x);
        setFloat("cameraDirection[1]", mGlobals.camDir.y);
        setFloat("cameraDirection[2]", mGlobals.camDir.z);
        mCmd->end();
        mDevice->execute(mCmd);
        mCmd->begin();
        //Disabling this until alina support referencing the default framebuffer
        mCmd->beginRenderPass(alina::RenderPassDesc().setFramebuffer(mOutputFb).setAttachmentsLoadOp({alina::RenderPassLoadOp::CLEAR}).setAttachmentsClearColors({{0,0,0,0}}));
        mCmd->beginSubPass(alina::SubPassDesc().setAttachments({alina::SubPassAttachment::COLOR}));
    }
}