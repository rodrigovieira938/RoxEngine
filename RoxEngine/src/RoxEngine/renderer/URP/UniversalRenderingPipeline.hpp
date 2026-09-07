#pragma once
#include <alina/alina.hpp>
#include <RoxEngine/renderer/RendereringPipeline.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>
#include <RoxEngine/renderer/Material.hpp>
#include <optional>
#include "RoxEngine/renderer/Transform.hpp"
namespace RoxEngine {
    class UniversalRenderingPipeline : public RenderingPipeline{
    public:
        UniversalRenderingPipeline(alina::Device device);
        void DrawMesh(RoxEngine::Mesh& mesh, RoxEngine::Material& material, std::optional<glm::mat4> transform = std::nullopt);
        void Begin(glm::mat4 viewMatrix = glm::mat4(1.0f), glm::mat4 projMatrix = glm::mat4(1.0f));
        void Render();
        alina::Buffer GetSharedUbo(const ReflectedResource& resource);
    private:
        struct Globals {
            glm::mat4 viewMatrix = glm::mat4(1.0f), projMatrix = glm::mat4(1.0f);
            glm::vec3 camPos = glm::vec3(0.f,0.f,-1.f);
            glm::vec3 camDir = glm::vec3(0.f,0.f,1.f);
        };

        std::unordered_map<std::string, alina::Buffer> mSharedUbos;

        Globals mGlobals;
        alina::Buffer mGlobalsUbo = nullptr;
        std::optional<ReflectedResource> mGlobalsUboReflection;

        alina::Framebuffer mOutputFb;

        AlinaGlue::InputLayoutPool mInputLayoutPool;
        AlinaGlue::GraphicsPipelinePool mGraphicsPipelinePool;
        alina::Device mDevice;
        alina::CommandList mCmd;
        alina::Framebuffer mFb;
        alina::Texture mFbTex;
    };
}