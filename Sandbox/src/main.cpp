#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/ecs/ecs.hpp"
#include "RoxEngine/renderer/Material.hpp"
#include "RoxEngine/renderer/Mesh.hpp"
#include "RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp"
#include "RoxEngine/slang/slang.hpp"
#include "RoxEngine/utils/Utils.hpp"
#include "alina/alina.hpp"
#include "slang-com-ptr.h"
#include "slang.h"
#include <RoxEngine/RoxEngine.hpp>
#include <glm/glm.hpp>

using namespace RoxEngine;

struct TestGame final : public Game {
    Ref<UniversalRenderingPipeline> pipeline;
    Mesh mesh;
    std::optional<Material> material;
    alina::Shader vertex_shader, fragment_shader;

    void Init() override {
        pipeline = CreateRef<UniversalRenderingPipeline>(Engine::Get()->GetWindow()->GetDevice());
        //Simple quad mesh
        mesh.SetPosition({
            {-1.0f, -1.0f, 0.0f},
            {-1.0f,  1.0f, 0.0f},
            { 1.0f,  1.0f, 0.0f},
            { 1.0f, -1.0f, 0.0f}
        });
        mesh.SetIndices({
            0, 2, 1,
    	    0, 3, 2
        });
        mesh.ChangedData();
        SlangLayer::Init();
        auto module = SlangLayer::CompileModule("res://shaders/basic.slang");
        auto moduleReflection = CreateRef<ModuleReflection>(SlangLayer::GetModuleReflection(module));
        std::array<slang::IComponentType*, 3> components = {
            module, 
            SlangLayer::GetModuleEntryPoint(module, SlangLayer::EntryPointType::VERTEX), 
            SlangLayer::GetModuleEntryPoint(module, SlangLayer::EntryPointType::FRAGMENT)
        };
        auto compositeComponent = SlangLayer::CreateCompositeComponentType(components);
        auto linkedProgram = SlangLayer::LinkModule(compositeComponent);
        auto vertex_shader_src = SlangLayer::GetModuleCode(linkedProgram);
        auto fragment_shader_src = SlangLayer::GetModuleCode(linkedProgram, 1);
        auto device = Engine::Get()->GetWindow()->GetDevice();
        vertex_shader = device->createShader(alina::ShaderType::VERTEX, vertex_shader_src.data(), vertex_shader_src.size());
        fragment_shader = device->createShader(alina::ShaderType::FRAGMENT, fragment_shader_src.data(), fragment_shader_src.size()); 
        material = Material(vertex_shader, fragment_shader, moduleReflection);
        material->Set("color", glm::vec3(1,2,3));
        material->Set("matrix", glm::mat4(1.0f));
    }
    void Update() override {
    }
    void Render() override {
        pipeline->DrawMesh(mesh, material.value());
        pipeline->Render();
        World::debugView();
    }
};

Scope<Game> CreateGame()
{
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    return Engine::Get()->Run(CreateGame());
}