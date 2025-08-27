#pragma once
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
    private:
        void begin();
        struct Globals {
            glm::mat4 viewMatrix = glm::mat4(1.0f), projMatrix = glm::mat4(1.0f);
            glm::vec3 camPos = glm::vec3(0,0,-1);
            glm::vec3 camDir = glm::vec3(0,0,1);
        };

        Globals mGlobals;
        alina::Buffer mGlobalsUbo;

        AlinaGlue::InputLayoutPool mInputLayoutPool;
        AlinaGlue::GraphicsPipelinePool mGraphicsPipelinePool;
        alina::Device mDevice;
        alina::CommandList mCmd;
        alina::Framebuffer mFb;
        alina::Texture mFbTex;
    };
}