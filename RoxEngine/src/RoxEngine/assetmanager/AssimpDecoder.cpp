#include "RoxEngine/renderer/Transform.hpp"
#include "RoxEngine/renderer/URP/MeshRendererer.hpp"
#include <RoxEngine/core/Logger.hpp>
#include <RoxEngine/renderer/Mesh.hpp>
#include <RoxEngine/assetmanager/AssimpDecoder.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <RoxEngine/filesystem/Filesystem.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RoxEngine::AssetManager {
    Mesh LoadMesh(const aiMesh* mesh) {
        Mesh _mesh;

        auto& indices = _mesh.GetIndices();
        for(int i = 0; i < mesh->mNumFaces; i++) {
            
            auto face = mesh->mFaces[i];
            if(face.mNumIndices != 3) {
                log::warn("Mesh {} has a face with {} indices, which is not supported by the current implementation of AssimpDecoder. Only faces with 3 indices are supported.", mesh->mName.C_Str(), face.mNumIndices);
            } else { //face.mNumIndices == 3
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }

        if(mesh->HasPositions()) {
            auto& positions = _mesh.GetPosition();
            positions.reserve(mesh->mNumVertices);
            for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
                auto& vertex = mesh->mVertices[i];
                positions.emplace_back(vertex.x, vertex.y, vertex.z);
            }
        }
        if(mesh->HasNormals()) {
            auto& normals = _mesh.GetNormals();
            normals.reserve(mesh->mNumVertices);
            for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
                auto& normal = mesh->mNormals[i];
                normals.emplace_back(normal.x, normal.y, normal.z);
            }
        }
        for(int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
            if(mesh->HasTextureCoords(i) && i < ShaderReflection::VertexBindingPoint::UV_MAX) {
                auto& texCoords = _mesh.GetUvs()[i];
                texCoords.reserve(mesh->mNumVertices);
                for(unsigned int j = 0; j < mesh->mNumVertices; j++) {
                    auto& texCoord = mesh->mTextureCoords[i][j];
                    texCoords.emplace_back(texCoord.x, texCoord.y);
                }
            } else if(mesh->HasTextureCoords(i)){
                log::warn("Mesh has more than {} UV channels, which are not supported by the current implementation of AssimpDecoder.", ShaderReflection::VertexBindingPoint::UV_MAX);
                break;
            }
        }
        _mesh.ChangedData();
        return _mesh;
    }
    void AssimpDecoder::Decode(const std::string_view& path)
    {
        auto file_contents = FileSystem::ReadFile(std::string(path));
        
        scene = importer.ReadFileFromMemory(file_contents.GetData(), file_contents.GetSize(), aiProcess_Triangulate | aiProcess_FlipUVs, FileSystem::GetFileExtension(std::string(path)).c_str());
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            log::error("Failed to load model: {}", importer.GetErrorString());
            return;
        }
        if(scene->HasMaterials()) {
            log::warn("Model has materials, which are not supported by the current implementation of AssimpDecoder.");
        }
        if(scene->HasLights()) {
            log::warn("Model has lights, which are not supported by the current implementation of AssimpDecoder.");
        }
        if(scene->HasTextures()) {
            log::warn("Model has textures, which are not supported by the current implementation of AssimpDecoder.");
        }
        if(scene->HasCameras()) {
            log::warn("Model has cameras, which are not supported by the current implementation of AssimpDecoder.");
        }
        if(scene->HasAnimations()) {
            log::warn("Model has animations, which are not supported by the current implementation of AssimpDecoder.");
        }
        if(scene->HasSkeletons()) {
            log::warn("Model has skeletons, which are not supported by the current implementation of AssimpDecoder.");
        }
        meshes.reserve(scene->mNumMeshes);
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* mesh = scene->mMeshes[i];
            meshes.push_back(LoadMesh(mesh));
        }
    }
    Entity AssimpDecoder::createEntityFromNode(Scene& scene,const aiNode* node, Material* material) {
        auto entity = scene.entity();
        entity.addComponent<Transform>(glm::vec3{0,0,0});
        entity.addComponent<DirtyTransform>();
        for(int i = 0; i < node->mNumChildren; i++) {
            auto childEntity = createEntityFromNode(scene, node->mChildren[i], material);
            childEntity.childOf(entity);
        }
        Mesh mesh;
        if(node->mNumMeshes > 1) {
            log::warn("Node {} has more than 1 mesh, which is not supported by the current implementation of AssimpDecoder. Only the first mesh will be used.", node->mName.C_Str());
        }
        log::info("Node {} has {} meshes.", node->mName.C_Str(), node->mNumMeshes);
        if(node->mNumMeshes > 0) {
            mesh = meshes[node->mMeshes[0]];
            entity.addComponent<MeshRenderer>(MeshRenderer(mesh, material));
            glm::mat4 matrix = glm::transpose(glm::make_mat4(&node->mTransformation[0][0]));
            glm::vec3 translation, scale, skew;
            glm::vec4 perspective;
            glm::quat rotation;

            bool success = glm::decompose(
                (glm::mat4)node->mTransformation[0][0],
                scale,
                rotation,
                translation,
                skew,
                perspective
            );
            auto t = Transform();
            t.translation = translation;
            t.rotation = glm::degrees(glm::eulerAngles(rotation));
            t.scale = scale;
            entity.addComponent<Transform>(t);
            entity.addComponent<DirtyTransform>();
        }
        return entity;
    }
    Entity AssimpDecoder::CreateEntities(Scene& _scene, Material* material) {
        return createEntityFromNode(_scene, scene->mRootNode, material);
    };
}