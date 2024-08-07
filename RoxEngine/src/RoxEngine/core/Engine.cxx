export module roxengine:engine;
import :utils;
import :window;

export namespace RoxEngine {
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