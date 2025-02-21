#pragma once

#include <RoxEngine/renderer/Mesh.hpp>
#include <RoxEngine/renderer/Transform.hpp>
#include <RoxEngine/renderer/Material.hpp>

namespace RoxEngine {
    class IRendererPipeline {
    public:
        virtual void DrawMesh(Mesh& mesh, Material* material, const Transform& transform = Transform()) = 0;
        virtual void SetCamera(glm::mat4 viewProj) = 0;    
    };
};