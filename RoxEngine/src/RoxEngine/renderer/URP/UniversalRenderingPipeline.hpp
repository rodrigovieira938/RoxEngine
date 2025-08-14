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

        AlinaGlue::InputLayoutPool mInputLayoutPool;
        AlinaGlue::GraphicsPipelinePool mGraphicsPipelinePool;
        alina::Device mDevice;
        alina::CommandList mCmd;
        alina::Framebuffer mFb;
        alina::Texture mFbTex;
    };
}