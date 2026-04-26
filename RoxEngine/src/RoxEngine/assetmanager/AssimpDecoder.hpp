#pragma once
#include "RoxEngine/renderer/Material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <RoxEngine/ecs/ecs.hpp>
#include <RoxEngine/renderer/Mesh.hpp>
#include <string_view>

namespace RoxEngine::AssetManager {
    class AssimpDecoder {
    public:
        static constexpr bool IsSameMimeType(const std::string_view& view)
		{
			return view.starts_with("model/");
		}
		void Decode(const std::string_view& path);
        Entity CreateEntities(Scene& scene, Material* material);
    private:
        Entity createEntityFromNode(Scene& scene,const aiNode* node, Material* material);
    public:
        std::vector<Mesh> meshes;
        Assimp::Importer importer;
        const aiScene* scene;
    };
};