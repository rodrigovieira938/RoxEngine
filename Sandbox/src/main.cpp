#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/ecs/ecs.hpp"
#include "RoxEngine/input/Input.hpp"
#include "RoxEngine/renderer/Material.hpp"
#include "RoxEngine/renderer/Mesh.hpp"
#include "RoxEngine/renderer/URP/MeshRendererer.hpp"
#include "RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp"
#include <RoxEngine/renderer/Transform.hpp>
#include <RoxEngine/renderer/slang/slang.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include "RoxEngine/utils/Utils.hpp"
#include "alina/alina.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "slang-com-ptr.h"
#include "slang.h"
#include <RoxEngine/RoxEngine.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace RoxEngine;

struct SimpleCamera {
    static constexpr glm::vec3 front = glm::vec3(0.f,0.f,-1.f);
    static constexpr glm::vec3 back = -front;
    static constexpr glm::vec3 up = glm::vec3(0.f,1.f,0.f);
    static constexpr glm::vec3 down = -up;
    static constexpr glm::vec3 right = glm::vec3(1.f,0.f,0.f);
    static constexpr glm::vec3 left = -right;
    float cameraSpeed;
    float fov;
    Transform transform;

    SimpleCamera() {
        Reset();
    }

    void Reset() {
        cameraSpeed = 0.05f;
        fov = 70.f;
        transform = {{0,0,3}};
    }

    glm::vec3 GetFront() const {
        return front;
    }
    glm::vec3 GetRight() const {
        return glm::normalize(glm::cross(GetFront(), up));
    }
    glm::vec3 GetUp() const {
        return glm::normalize(glm::cross(GetRight(), GetFront()));
    }
    glm::mat4 GetViewMatrix() {
        glm::quat rotation = glm::quat(transform.rotation);
        return glm::lookAt(transform.translation, transform.translation + rotation * GetFront(), rotation * GetUp());
    }
    glm::mat4 GetProjectionMatrix(float aspectRatio) {
        return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
    }
    void ProcessInput() {
        glm::quat q = glm::quat(transform.rotation);
        
        if (Input::IsKeyDown(Key::W))
            transform.translation += cameraSpeed * (q * front);
        if (Input::IsKeyDown(Key::S))
            transform.translation += cameraSpeed * (q * back);
        if (Input::IsKeyDown(Key::A))
            transform.translation += cameraSpeed * (q * left);
        if (Input::IsKeyDown(Key::D))
            transform.translation += cameraSpeed * (q * right);
        if(Input::IsKeyDown(Key::Q))
            transform.translation += cameraSpeed * (q * up);
        if(Input::IsKeyDown(Key::E))
            transform.translation += cameraSpeed * (q * down);

        float angleSpeed = glm::radians(1.0f); // rotation per frame


        // Rotate around local axes
        if (Input::IsKeyDown(Key::Left))  q = glm::angleAxis(angleSpeed, glm::vec3(0,1,0)) * q;
        if (Input::IsKeyDown(Key::Right)) q = glm::angleAxis(-angleSpeed, glm::vec3(0,1,0)) * q;
        if (Input::IsKeyDown(Key::Up))    q = glm::angleAxis(angleSpeed, glm::vec3(1,0,0)) * q;
        if (Input::IsKeyDown(Key::Down))  q = glm::angleAxis(-angleSpeed, glm::vec3(1,0,0)) * q;


        // Store back as Euler for your GetMatrix()
        transform.rotation = glm::eulerAngles(q);

        if(Input::IsKeyPressed(Key::R)) {
            Reset();
        }
    }
};

struct TestGame final : public Game {
    Ref<UniversalRenderingPipeline> pipeline;
    Mesh mesh;
    std::optional<Material> material;
    std::optional<Material> material2;
    alina::Shader vertex_shader, fragment_shader;
    SimpleCamera camera;
    Scene scene;
    Query meshRendererQuery;

    TestGame() : scene(World::createScene("TestGame")), meshRendererQuery(QueryBuilder().with<MeshRenderer>().build()) {
    }

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
        std::array<slang::IComponentType*, 3> components = {
            module, 
            SlangLayer::GetModuleEntryPoint(module, SlangLayer::EntryPointType::VERTEX), 
            SlangLayer::GetModuleEntryPoint(module, SlangLayer::EntryPointType::FRAGMENT)
        };
        auto compositeComponent = SlangLayer::CreateCompositeComponentType(components);
        auto linkedProgram = SlangLayer::LinkModule(compositeComponent);
        auto moduleReflection = CreateRef<ModuleReflection>(SlangLayer::GetProgramReflection(linkedProgram));
        auto vertex_shader_src = SlangLayer::GetModuleCode(linkedProgram);
        auto fragment_shader_src = SlangLayer::GetModuleCode(linkedProgram, 1);
        auto device = Engine::Get()->GetWindow()->GetDevice();
        vertex_shader = device->createShader(alina::ShaderType::VERTEX, vertex_shader_src.data(), vertex_shader_src.size());
        fragment_shader = device->createShader(alina::ShaderType::FRAGMENT, fragment_shader_src.data(), fragment_shader_src.size()); 
        material = Material(vertex_shader, fragment_shader, moduleReflection);
        material->Set("color", glm::vec3(0.5,1,0.3));
        material2 = Material(vertex_shader, fragment_shader, moduleReflection);
        material2->Set("color", glm::vec3(1, 0.5,0.3));
        auto cube = scene.entity("Cube");
        cube.addComponent<Transform>(glm::vec3{0,0,0});
        cube.addComponent<DirtyTransform>();
        cube.addComponent<MeshRenderer>(mesh, &material.value());

        auto cube2 = scene.entity("Cube2");
        cube2.childOf(cube);
        cube2.addComponent<Transform>(glm::vec3{0.5,0,0});
        cube2.addComponent<DirtyTransform>();
        cube2.addComponent<MeshRenderer>(mesh, &material2.value());
    }
    WorldTransform GetWorldTransform(Entity e) {
        // If entity has no transform, return identity
        if (!e.hasComponent<Transform>()) 
            return WorldTransform(1.0f);

        // Ensure the entity has a WorldTransform component
        if (!e.hasComponent<WorldTransform>()) 
            e.addComponent<WorldTransform>(e.getComponent<Transform>()->GetMatrix());

        // If the transform is not dirty, return cached WorldTransform
        if (!e.hasComponent<DirtyTransform>()) 
            return *e.getComponent<WorldTransform>();

        // Remove dirty flag since we are updating
        e.removeComponent<DirtyTransform>();

        // Get parent world transform
        WorldTransform parentWT(1.0f);
        auto parent = e.parent();
        if (parent.exists()) {
            parentWT = GetWorldTransform(parent);
        }
        auto worldTransform = WorldTransform(parentWT * e.getComponent<Transform>()->GetMatrix());

        // Update entity's world transform
        *e.getComponent<WorldTransform>() = worldTransform;

        if(e.hasComponent<MeshRenderer>()) {
            auto mr = e.getComponent<MeshRenderer>();
            mr->material->Set("@InstanceData", worldTransform);
        }

        e.children([](Entity e){
            e.addComponent<DirtyTransform>();
        });

        return *e.getComponent<WorldTransform>();
    };
    void Update() override {
        camera.ProcessInput();
    }
    void Render() override {
        pipeline->Begin(camera.GetViewMatrix(), camera.GetProjectionMatrix(1.0f));
        meshRendererQuery.each([&](Entity entity, QueryIter& iter){
            auto meshRenderer = (MeshRenderer*)iter.get(0);
            auto worldTransform = GetWorldTransform(entity);
            //FIXME: since the transform is passed by a ubo for the whole frame it gets overriden with the last transform
            pipeline->DrawMesh(meshRenderer->mesh, *meshRenderer->material);
        });
        pipeline->Render();
        World::debugView();
        ImGui::Begin("Camera info");
        ImGui::Text("Position: %s", glm::to_string(camera.transform.translation).c_str());
        ImGui::SliderFloat3("Rotation", &camera.transform.rotation.x, -180.0f, 180.0f);
        ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 90.0f);
        ImGui::SliderFloat("Camera Speed", &camera.cameraSpeed, 0.1f, 10.0f);
        ImGui::End();

        auto selectedEntity = World::getSelectedEntity();
        ImGui::Begin("Transform Info");
        if(selectedEntity.exists()) {
            if(selectedEntity.hasComponent<Transform>())
            {
                auto transform = selectedEntity.getComponent<Transform>();
                ImGui::DragFloat3("Translation", &transform->translation.x);
                ImGui::SliderFloat3("Rotation", &transform->rotation.x, -180.0f, 180.0f);
                ImGui::DragFloat3("Scale", &transform->scale.x);
                ImGui::Text("%s", std::string(selectedEntity.name()).c_str());
                selectedEntity.addComponent<DirtyTransform>();
            }
        }
        ImGui::End();
    }
};

Scope<Game> CreateGame()
{
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    #ifdef SANDBOX_DEBUG_PWD
        std::filesystem::current_path(SANDBOX_DEBUG_PWD);
    #endif

    return Engine::Get()->Run(CreateGame());
}