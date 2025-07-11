#pragma once
#include <RoxEngine/utils/Utils.hpp>
#include <RoxEngine/core/Window.hpp>
namespace RoxEngine {
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
        inline Ref<Window> GetWindow() {return mWindow;};
    private:
        Ref<Window> mWindow;

        void DrawDebugInfo();

        Engine() = default;
    };
}