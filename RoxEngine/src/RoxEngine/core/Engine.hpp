#pragma once
#include <RoxEngine/utils/Utils.hpp>
#include <RoxEngine/core/Window.hpp>
#include <RoxEngine/core/Logger.hpp>
namespace RoxEngine {
    class RenderingPipeline;
    class Game {
    public:
        virtual ~Game() = default;
        virtual void Init() {};
        virtual void Update() {};
        virtual void Render() {};
    };
    class Engine {
    public:
        static Engine* Get();
        //TODO: Make this return an error
        int Run(Scope<Game> game);
        void SetRenderingPipeline(RenderingPipeline* pipeline) {
            if(mRenderingPipeline) {
                log::error("Rendering pipeline already set! Overriding its not possible without a restart!");
                return;
            }
            mRenderingPipeline = pipeline;
        }
        RenderingPipeline* GetRenderingPipeline() {return mRenderingPipeline;}
        inline Ref<Window> GetWindow() {return mWindow;};
    private:
        Ref<Window> mWindow;
        RenderingPipeline* mRenderingPipeline = nullptr;

        void DrawDebugInfo();

        Engine() = default;
    };
}