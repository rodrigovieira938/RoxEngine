#pragma once
#include <RoxEngine/renderer/GraphicsContext.hpp>

namespace RoxEngine::GL {
    class GraphicsContext final : public ::RoxEngine::GraphicsContext {
    public:
        GraphicsContext();
        ~GraphicsContext();

        virtual void VClearColor(float r,float g,float b,float a = 1.0) final;
        virtual void VClearScreen(ClearScreenMask bufferBits) final;
        virtual void VUseMaterial(Ref<::RoxEngine::Material> material) final;
        virtual void VDraw(Ref<VertexArray>, size_t indexCount) final;
    };
}