#pragma once
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <RoxEngine/renderer/VertexArray.hpp>
#include <RoxEngine/renderer/Buffer.hpp>
#include <RoxEngine/renderer/Material.hpp>
#include <RoxEngine/renderer/Mesh.hpp>
namespace RoxEngine {
    //Basic renderer implementation
    class IRenderer
    {
        //Returns camera's id for this frame
        virtual size_t SetCamera(glm::mat4 viewProj, glm::mat4 camPos) = 0;
        //CameraID=0 for last set camera
        virtual void DrawMesh(Mesh mesh, size_t cameraID = 0) = 0;
        virtual void Render() const = 0;
        virtual void Clear() = 0;
    };
    class DefaultRenderer : public IRenderer
    {
    private:
        struct Camera
        {
            glm::mat4 viewProj, camPos;
        };
        struct Batch
        {
            size_t camIndex;
            Ref<VertexArray> va;
        };
        std::vector<Camera> cams;
        std::vector<Batch> batches;

        bool debugWireMesh = false;
        bool debugNormalRendering = true;
        glm::vec3 debugWireMeshColor = {1,0,1};
        float debugWireMeshLineSize = 3;
    public:
        std::shared_ptr<Material> debugMaterial = nullptr;

        size_t SetCamera(glm::mat4 viewProj, glm::mat4 camPos) override
        {
            cams.push_back({ viewProj, camPos });
            return cams.size() - 1;
        }
        void DrawMesh(Mesh mesh, size_t cameraID = 0) override
        {
            Batch b;
            if (cameraID == 0)
                b.camIndex = cams.size() - 1;
            else
                b.camIndex = cameraID;
            b.va = VertexArray::Create();
            b.va->SetIndexBuffer(IndexBuffer::Create(mesh.indices.data(), mesh.indices.size()));
            #define SET_VA(attr, sizef) if(mesh.attr.size() > 0) { auto vb = VertexBuffer::Create(mesh.attr.data(), mesh.attr.size() * (sizef));vb->SetLayout({ {#attr, ShaderDataType::Float3} }); b.va->AddVertexBuffer(vb);}
            SET_VA(position, sizeof(glm::vec3));
            SET_VA(uv, sizeof(glm::vec2));
            SET_VA(uv2, sizeof(glm::vec2));
            SET_VA(uv3, sizeof(glm::vec2));
            SET_VA(uv4, sizeof(glm::vec2));
            SET_VA(uv5, sizeof(glm::vec2));
            SET_VA(uv6, sizeof(glm::vec2));
            SET_VA(uv7, sizeof(glm::vec2));
            SET_VA(uv8, sizeof(glm::vec2));
            SET_VA(normal, sizeof(glm::vec3));
            SET_VA(color, sizeof(glm::vec4));
            batches.emplace_back(b);
        }
        void DebugMenu();
        void Render() const override;
        void Clear() override
        {
            cams.clear();
            batches.clear();
        }
        virtual ~DefaultRenderer() = default;
    };
}