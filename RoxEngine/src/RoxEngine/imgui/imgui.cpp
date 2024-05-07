#include "imgui.hpp"
#include "RoxEngine/platforms/GLFW/GLFWWindow.hpp"
#include <RoxEngine/core/Engine.hpp>
#include <imgui/backends/imgui_impl_opengl3.cpp>
#include <imgui/backends/imgui_impl_glfw.cpp>
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_tables.cpp>
#include <imgui/imgui_widgets.cpp>
#include <imgui/imgui.cpp>

namespace RoxEngine {
    void ImGuiLayer::Init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        #if defined(IMGUI_IMPL_OPENGL_ES2)
            const char* glsl_version = "#version 100";
        #elif defined(__APPLE__)
            const char* glsl_version = "#version 150";
        #else
            const char* glsl_version = "#version 130";
        #endif

        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)std::static_pointer_cast<GLFW::Window>(Engine::Get()->GetWindow())->mWindow, false);
        bool a = ImGui_ImplOpenGL3_Init(glsl_version);
    }
    void ImGuiLayer::NewFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void ImGuiLayer::Render() {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize((GLFWwindow*)std::static_pointer_cast<GLFW::Window>(Engine::Get()->GetWindow())->mWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
    void ImGuiLayer::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}