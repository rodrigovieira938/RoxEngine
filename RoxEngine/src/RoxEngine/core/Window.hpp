#pragma once

#include <string>
namespace RoxEngine {
    class Window {
    public:
        virtual ~Window() = default;
        virtual bool IsOpen() = 0;
        virtual void PollEvents() = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual std::string GetTitle() = 0;
        virtual void SetSize(int width, int height) = 0;
        virtual std::pair<int,int> GetSize() = 0;
        virtual void SetMaximized(bool val) = 0;
        virtual bool IsMaximized() = 0;
    };
}