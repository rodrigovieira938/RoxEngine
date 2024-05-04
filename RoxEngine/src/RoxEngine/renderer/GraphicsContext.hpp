#pragma once

#include "RoxEngine/renderer/VertexArray.hpp"
#include "RoxEngine/utils/Memory.h"
namespace RoxEngine {
    enum class RendererApi {
        //NONE = -1, /*doesnt exist because there is no need to*/
        OPENGL
    };
    using ClearScreenMask = unsigned int;
    namespace ClearScreen {
        enum ClearScreen {
            COLOR   = 0b001,
            DEPTH   = 0b010,
            STENCIL = 0b100,
        };
    }
    class GraphicsContext {
    public:
        virtual ~GraphicsContext() = default;

        static void ClearColor(float r,float g,float b,float a = 1.0);
        static void ClearScreen(ClearScreenMask bufferBits = ClearScreen::COLOR | ClearScreen::DEPTH | ClearScreen::STENCIL);
        //TODO: add common commands that the renderer api should do e.g: Draw, clear color, etc...
    protected:
        GraphicsContext() = default;
    private:
        friend class Engine;
        
        //Initializes the singleton to the desired api
        static void Init(RendererApi api);
        static void Shutdown();

        virtual void VClearColor(float r,float g,float b,float a = 1.0) = 0;
        virtual void VClearScreen(ClearScreenMask bufferBits) = 0;
    };
}