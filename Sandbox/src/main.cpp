#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/input/Input.hpp"
#include "RoxEngine/renderer/DefaultRendererPipeline/DefaultRendererPipeline.hpp"
#include <glm/glm.hpp>
#include <string>
#include <RoxEngine/roxengine.hpp> //better for lsps
#include <RoxEngine/renderer/DefaultRendererPipeline/Rendeder.hpp>
import roxengine;
using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<VertexArray> va;
    Ref<Shader> shader;
    Ref<Material> material;
    Ref<Framebuffer> fb;
    DefaultRendererPipeline* rendererPipeline;

    void Init() override {
        rendererPipeline = new DefaultRendererPipeline();
        Engine::Get()->SetRendererPipeline(rendererPipeline);

        Mesh mesh;
        mesh.position= {
            {-0.5f, -0.5f,  0.5f},  // bottom-left
			{ 0.5f, -0.5f,  0.5f},  // bottom-right
			{ 0.5f,  0.5f,  0.5f},  // top-right
			{-0.5f,  0.5f,  0.5f},  // top-left

			// Back face
			{-0.5f, -0.5f, -0.5f}, // bottom-left
			{ 0.5f, -0.5f, -0.5f}, // bottom-right
			{ 0.5f,  0.5f, -0.5f}, // top-right
			{-0.5f,  0.5f, -0.5f}, // top-left

			// Left face
			{-0.5f, -0.5f, -0.5f}, // bottom-back
			{-0.5f, -0.5f,  0.5f}, // bottom-front
			{-0.5f,  0.5f,  0.5f}, // top-front
			{-0.5f,  0.5f, -0.5f}, // top-back

			// Right face
			{ 0.5f, -0.5f, -0.5f},  // bottom-back
			{ 0.5f, -0.5f,  0.5f},  // bottom-front
			{ 0.5f,  0.5f,  0.5f},  // top-front
			{ 0.5f,  0.5f, -0.5f},  // top-back

			// Top face
			{-0.5f,  0.5f,  0.5f},  // front-left
			{ 0.5f,  0.5f,  0.5f},  // front-right
			{ 0.5f,  0.5f, -0.5f},  // back-right
			{-0.5f,  0.5f, -0.5f},  // back-left

			// Bottom face
			{-0.5f, -0.5f,  0.5f}, // front-left
			{ 0.5f, -0.5f,  0.5f}, // front-right
			{ 0.5f, -0.5f, -0.5f}, // back-right
			{-0.5f, -0.5f, -0.5f}  // back-left
        };
    	mesh.indices = {
            // Front face
		    0, 1, 2,  0, 2, 3,
		    // Back face
		    4, 5, 6,  4, 6, 7,
		    // Left face
		    8, 9, 10,  8, 10, 11,
		    // Right face
		    12, 13, 14,  12, 14, 15,
		    // Top face
		    16, 17, 18,  16, 18, 19,
		    // Bottom face
		    20, 21, 22,  20, 22, 23
        };

        fb = Framebuffer::Create(800, 800, {FramebufferColorTexFormat::RGB32}, FramebufferDepthTexFormat::D24UNS8U);
        shader = Shader::Create("res://shaders/basic.slang");
        material = Material::Create(shader, Material::EntryPointInfo{ "basic_vmain", "basic_fmain" });
        rendererPipeline->mRenderer.debugMaterial = Material::Create(shader, Material::EntryPointInfo{ "basic_vmain", "basic_fmain" });
        glm::mat4 a = { 1.0f };
        glm::vec3 color = { 0,0,1 };
    	rendererPipeline->SetCamera({});
        rendererPipeline->DrawMesh(mesh, material.get());
    	rendererPipeline->mRenderer.debugMaterial->GetUbo("Uniforms")->Set("matrix", &a[0][0], sizeof(glm::mat4));
        rendererPipeline->mRenderer.debugMaterial->GetUbo("Uniforms")->Set("color", &color.r, sizeof(color));

        if(auto ubo = material->GetUbo("Uniforms"))
        {
            glm::vec3 color = { 255.f / 255.f,223.f / 255.f,214.f / 255.f, };
            glm::mat4 matrix = glm::mat4(1.f);

            ubo->Set("color", &color.x, sizeof(glm::vec3));
            ubo->Set("matrix", &matrix[0][0], sizeof(glm::mat4));
        }
    }
    void Update() override {
    }
    void Render() override {
        GraphicsContext::ClearScreen();
        GraphicsContext::UseMaterial(material);
        rendererPipeline->mRenderer.DebugMenu();
        rendererPipeline->Render();
    }
};

Scope<Game> CreateGame() {
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    return Engine::Get()->Run(CreateGame());
}