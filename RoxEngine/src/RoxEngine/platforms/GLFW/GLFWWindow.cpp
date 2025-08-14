#include "RoxEngine/profiler/profiler.hpp"
#include "alina/alina.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>
#include <RoxEngine/platforms/GLFW/GLFWWindow.hpp>
#include <alina/opengl.hpp>
#define GET_WINDOW ((GLFWwindow*)mWindow)

namespace RoxEngine::GLFW {
    Window::Window(RendererBackend backend) {
        PROFILER_FUNCTION();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        mWindow = glfwCreateWindow(600, 600, "RoxEngine - APP", nullptr, nullptr);
        glfwMakeContextCurrent(GET_WINDOW);
        //Backend can only be opengl for now
        mDevice = Ref<alina::IDevice>(alina::opengl::CreateDevice(glfwGetProcAddress));
    }
    Window::~Window() {
        PROFILER_FUNCTION();
        glfwDestroyWindow(GET_WINDOW);
    }
    bool Window::IsOpen() {
        PROFILER_FUNCTION();
        return !glfwWindowShouldClose(GET_WINDOW);
    }
    void Window::PollEvents() {
        PROFILER_FUNCTION();
        glfwPollEvents();
    }
    void Window::SetTitle(const std::string& title) {
        PROFILER_FUNCTION();
        glfwSetWindowTitle(GET_WINDOW, title.c_str());
    }
    std::string Window::GetTitle() {
        PROFILER_FUNCTION();
        return std::string(glfwGetWindowTitle(GET_WINDOW));
    }
    void Window::SetSize(int width, int height) {
        PROFILER_FUNCTION();
        glfwSetWindowSize(GET_WINDOW, width, height);
    }
    std::pair<int,int> Window::GetSize() {
        PROFILER_FUNCTION();
        std::pair<int,int> size;
        glfwGetWindowSize(GET_WINDOW, &size.first, &size.second);
        return size;
    }
    void Window::SetMaximized(bool val) {
        PROFILER_FUNCTION();
        if(val) 
            glfwMaximizeWindow(GET_WINDOW);
        else
            glfwRestoreWindow(GET_WINDOW);
    }
    bool Window::IsMaximized() {
        PROFILER_FUNCTION();
        return glfwGetWindowAttrib(GET_WINDOW, GLFW_MAXIMIZED);
    }
}