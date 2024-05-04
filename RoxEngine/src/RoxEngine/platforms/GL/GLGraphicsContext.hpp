#pragma once
#include <RoxEngine/renderer/GraphicsContext.hpp>

namespace RoxEngine::GL {
    class GraphicsContext final : public ::RoxEngine::GraphicsContext {
    public:
        GraphicsContext();
        ~GraphicsContext();

        void VClearColor(float r,float g,float b,float a = 1.0);
        void VClearScreen(ClearScreenMask bufferBits);
    };
}