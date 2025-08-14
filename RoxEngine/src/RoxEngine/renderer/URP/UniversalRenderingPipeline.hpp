#pragma once
#include <alina/alina.hpp>
#include <RoxEngine/renderer/RendereringPipeline.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>
namespace RoxEngine {
    class UniversalRenderingPipeline : public RenderingPipeline{
    public:
        UniversalRenderingPipeline(alina::Device device);
        void DrawMesh(RoxEngine::Mesh& mesh);
        void Render();
    private:
        AlinaGlue::InputLayoutPool mInputLayoutPool;
        AlinaGlue::GraphicsPipelinePool mGraphicsPipelinePool;
        alina::Device mDevice;
        alina::CommandList mCmd;
    };
}