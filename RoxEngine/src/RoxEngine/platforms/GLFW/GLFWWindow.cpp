#include "GLFWWindow.hpp"
#include <GLFW/glfw3.h>

#define GET_WINDOW ((GLFWwindow*)mWindow)

namespace RoxEngine::GLFW {
    Window::Window() {
        mWindow = glfwCreateWindow(600, 600, "RoxEngine - APP", nullptr, nullptr);
        glfwMakeContextCurrent(GET_WINDOW);
    }
    Window::~Window() {
        glfwDestroyWindow(GET_WINDOW);
    }
    bool Window::IsOpen() {
        return !glfwWindowShouldClose(GET_WINDOW);
    }
    void Window::PollEvents() {
        glfwPollEvents();
    }
    void Window::SetTitle(const std::string& title) {
        glfwSetWindowTitle(GET_WINDOW, title.c_str());
    }
    std::string Window::GetTitle() {
        return std::string(glfwGetWindowTitle(GET_WINDOW));
    }
    void Window::SetSize(int width, int height) {
        glfwSetWindowSize(GET_WINDOW, width, height);
    }
    std::pair<int,int> Window::GetSize() {
        std::pair<int,int> size;
        glfwGetWindowSize(GET_WINDOW, &size.first, &size.second);
        return size;
    }
    void Window::SetMaximized(bool val) {
        if(val) 
            glfwMaximizeWindow(GET_WINDOW);
        else
            glfwRestoreWindow(GET_WINDOW);
    }
    bool Window::IsMaximized() {
        return glfwGetWindowAttrib(GET_WINDOW, GLFW_MAXIMIZED);
    }
}