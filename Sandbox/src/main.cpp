#include "RoxEngine/assetmanager/AssetManager.hpp"
#include "RoxEngine/assetmanager/AssimpDecoder.hpp"
#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/ecs/ecs.hpp"
#include "RoxEngine/filesystem/Filesystem.hpp"
#include "RoxEngine/input/Input.hpp"
#include "RoxEngine/renderer/Material.hpp"
#include "RoxEngine/renderer/Mesh.hpp"
#include <RoxEngine/renderer/slang/filesystem.hpp>
#include "RoxEngine/renderer/URP/MeshRendererer.hpp"
#include "RoxEngine/renderer/URP/UniversalRenderingPipeline.hpp"
#include <RoxEngine/renderer/Transform.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include "RoxEngine/utils/Utils.hpp"
#include "alina/alina.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "shaderc/MaterialBinary.hpp"
#include "slang-com-ptr.h"
#include "slang.h"
#include <RoxEngine/RoxEngine.hpp>
#include <glm/glm.hpp>
#include <sstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <assimp/scene.h>
#include <shaderc/BatchCompiler.hpp>

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
    std::optional<Material> material;
    alina::Shader vertex_shader, fragment_shader;
    SimpleCamera camera;
    Scene scene;
    Query meshRendererQuery;

    TestGame() : scene(World::createScene("TestGame")), meshRendererQuery(QueryBuilder().with<MeshRenderer>().build()) {
    }

    void Init() override {
        pipeline = CreateRef<UniversalRenderingPipeline>(Engine::Get()->GetWindow()->GetDevice());
        {
            std::string shaderSource = FileSystem::ReadTextFile("res://shaders/basic.slang");
            MaterialCompiler::sFilesystem = new SlangLayer::filesystem();
            BatchCompiler compiler({SLANG_SPIRV}, {""});
            BatchCompileResult result = compiler.compileSource("res://shaders/basic.slang", shaderSource);
            
            if(result.variants[0].diagnostics.size() > 0) {
                log::error("Shader compilation failed: {}", result.variants[0].diagnostics);
                exit(1);
            }
            
            std::stringstream stream(std::ios::in | std::ios::out);
            std::string error;
            if(!writeMaterialBinary(stream, result, &error)) {
                log::error("Failed to write material binary: {}", error);
                exit(1);
            }
            LoadedMaterial loadedMaterial;
            if(!readMaterialBinary(stream, loadedMaterial, &error)) {
                log::error("Failed to read material binary: {}", error);
                exit(1);
            }
            
            material = Material(loadedMaterial, "default");
        }

        AssetManager::AssimpDecoder decoder;
        if(AssetManager::Load("res://models/cottage_fbx.fbx", decoder)) {
            decoder.CreateEntities(scene, &material.value());
        }

        material->Set("color", glm::vec3(0.5,0.2,0.3));
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