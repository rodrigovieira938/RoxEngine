#pragma once
#include "RoxEngine/slang/slang.hpp"
#include <alina/alina.hpp>
#include <RoxEngine/renderer/RendereringPipeline.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>
#include <RoxEngine/renderer/Material.hpp>
namespace RoxEngine {
    class UniversalRenderingPipeline : public RenderingPipeline{
    public:
        UniversalRenderingPipeline(alina::Device device);
        void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material);
        void Render();
        void SetViewMatrix(glm::mat4 value) {mGlobals.viewMatrix = value;mGlobals.needUpdate = true;}
        void SetProjMatrix(glm::mat4 value) {mGlobals.projMatrix = value;mGlobals.needUpdate = true;}
    private:
        void begin();
        struct Globals {
            glm::mat4 viewMatrix = glm::mat4(1.0f), projMatrix = glm::mat4(1.0f);
            glm::vec3 camPos = glm::vec3(0.f,0.f,-1.f);
            glm::vec3 camDir = glm::vec3(0.f,0.f,1.f);

            bool needUpdate = true;
        };

        Globals mGlobals;
        alina::Buffer mGlobalsUbo;
        ModuleReflection mGlobalsUboReflection;
        size_t mGlobalUboIndex;

        alina::Framebuffer mOutputFb;

        AlinaGlue::InputLayoutPool mInputLayoutPool;
        AlinaGlue::GraphicsPipelinePool mGraphicsPipelinePool;
        alina::Device mDevice;
        alina::CommandList mCmd;
        alina::Framebuffer mFb;
        alina::Texture mFbTex;
    };
}