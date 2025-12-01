#pragma once
#include <RoxEngine/renderer/Mesh.hpp>
#include <RoxEngine/renderer/Material.hpp>

namespace RoxEngine {
    struct MeshRenderer {
        Mesh mesh;
        Material* material;
    
        bool operator ==(const MeshRenderer& other) const {
            //TODO: add comparison to mesh and compare it
            return material == other.material;
        }
    };
}