module;
#include <glad/gl.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#undef glActiveTexture           
#undef glAttachShader            
#undef glBindBuffer              
#undef glBindSampler             
#undef glBindTexture             
#undef glBindVertexArray         
#undef glBlendEquation           
#undef glBlendEquationSeparate   
#undef glBlendFuncSeparate       
#undef glBufferData              
#undef glBufferSubData           
#undef glClear                   
#undef glClearColor              
#undef glCompileShader           
#undef glCreateProgram           
#undef glCreateShader            
#undef glDeleteBuffers           
#undef glDeleteProgram           
#undef glDeleteShader            
#undef glDeleteTextures          
#undef glDeleteVertexArrays      
#undef glDetachShader            
#undef glDisable                 
#undef glDisableVertexAttribArray
#undef glDrawElements            
#undef glDrawElementsBaseVertex  
#undef glEnable                  
#undef glEnableVertexAttribArray 
#undef glFlush                   
#undef glGenBuffers              
#undef glGenTextures             
#undef glGenVertexArrays         
#undef glGetAttribLocation       
#undef glGetError                
#undef glGetIntegerv             
#undef glGetProgramInfoLog       
#undef glGetProgramiv            
#undef glGetShaderInfoLog        
#undef glGetShaderiv             
#undef glGetString               
#undef glGetStringi              
#undef glGetUniformLocation      
#undef glGetVertexAttribPointerv 
#undef glGetVertexAttribiv       
#undef glIsEnabled               
#undef glIsProgram               
#undef glLinkProgram             
#undef glPixelStorei             
#undef glPolygonMode             
#undef glReadPixels              
#undef glScissor                 
#undef glShaderSource            
#undef glTexImage2D              
#undef glTexParameteri           
#undef glUniform1i               
#undef glUniformMatrix4fv        
#undef glUseProgram              
#undef glVertexAttribPointer     
#undef glViewport
#include <imgui/backends/imgui_impl_opengl3.cpp>
#include <imgui/backends/imgui_impl_glfw.cpp>
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_tables.cpp>
#include <imgui/imgui_widgets.cpp>
#include <imgui/imgui.cpp>
#include <imgui/misc/cpp/imgui_stdlib.cpp>
#include <memory>
module roxengine;

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