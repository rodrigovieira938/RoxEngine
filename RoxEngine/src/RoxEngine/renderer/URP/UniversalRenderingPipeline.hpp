#pragma once
#include <alina/alina.hpp>
#include <RoxEngine/renderer/RendereringPipeline.hpp>

namespace RoxEngine {
    class UniversalRenderingPipeline : public RenderingPipeline{
    public:
        UniversalRenderingPipeline(Ref<alina::IDevice> device);
        void DrawMesh(RoxEngine::Mesh& mesh);
        void Render();
    private:
        Ref<alina::IDevice> mDevice;
        Ref<alina::ICommandList> mCmd;
    };
}