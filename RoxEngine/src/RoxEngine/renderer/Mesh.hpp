#pragma once
#include <RoxEngine/utils/Utils.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace RoxEngine {
    class Mesh {
    public:
        Mesh() : mData(CreateRef<Data>()) {}
        Mesh clone() {return Mesh(mData); };
        void SetPosition(std::vector<glm::vec3> data) {mData->position = data;}
        void SetUvs(std::vector<glm::vec2> data) {mData->uvs = data;}
        void SetNormals(std::vector<glm::vec3> data) {mData->normals = data;}
        void SetIndices(std::vector<uint32_t> data) {mData->indices = data;}
        std::vector<glm::vec3>& GetPosition() {return mData->position;}
        std::vector<glm::vec2>& GetUvs() {return mData->uvs;}
        std::vector<glm::vec3>& GetNormals() {return mData->normals;}
        std::vector<uint32_t>& GetIndices() {return mData->indices;}
    private:
        struct Data {
            std::vector<glm::vec3> position;
            std::vector<glm::vec2> uvs;
            std::vector<glm::vec3> normals;
            std::vector<uint32_t> indices;
        };

        Mesh(Ref<Data>& data){
            mData = CreateRef<Data>();
            mData->position = data->position;
            mData->uvs = data->uvs;
            mData->normals = data->normals;
        }

        Ref<Data> mData; 
    };
}