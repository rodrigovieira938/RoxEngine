#pragma once
#include "alina/alina.hpp"
#include <RoxEngine/utils/Utils.hpp>
#include <array>
#include <glm/glm.hpp>
#include <vector>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>
#include <RoxEngine/renderer/ShaderReflection.hpp>

namespace RoxEngine {
    class Mesh {
    public:
        struct Data {
        private:
            template<typename T, std::size_t size, std::size_t index>
            struct FieldAliasWorkaround {
            public:
                T* operator ->() {
                    return &data[index];
                }
                operator T&() {
                    return data[index];
                }
                operator const T&() const {
                    return data[index];
                }
                FieldAliasWorkaround& operator=(const T& value) {
                    data[index] = value;
                    return *this;
                }

                FieldAliasWorkaround& operator=(T&& value) {
                    data[index] = std::move(value);
                    return *this;
                }
            private:
                std::array<T, size> data;  
            };
        public:
            Data() {
                new(&uv) FieldAliasWorkaround<std::vector<glm::vec2>, ShaderReflection::VertexBindingPoint::UV_MAX, 0>();
            }
            ~Data() {
                uv.~FieldAliasWorkaround();
            }
            std::vector<glm::vec3> position;
            union {
                std::array<std::vector<glm::vec2>, ShaderReflection::VertexBindingPoint::UV_MAX> uvs;
                FieldAliasWorkaround<std::vector<glm::vec2>, ShaderReflection::VertexBindingPoint::UV_MAX, 0> uv;
                FieldAliasWorkaround<std::vector<glm::vec2>, ShaderReflection::VertexBindingPoint::UV_MAX, 1> uv1;
            };
            std::vector<glm::vec3> normals;
            std::vector<uint32_t> indices;

            alina::Buffer position_vb, uvs_vb, normals_vb, indices_vb;
            
            void BakeGPUResources(alina::Device device, AlinaGlue::InputLayoutPool* inputLayoutPool = nullptr) {
                auto cmd = device->createCommandList();
                size_t vertexCount = position.size();
                position_vb = device->createBuffer(alina::BufferDesc().setDebugName("MESH - Position VB").setType(alina::BufferType::VERTEX));
                indices_vb = device->createBuffer(alina::BufferDesc().setDebugName("MESH - IndexBuffer").setType(alina::BufferType::INDEX));
                cmd->begin();
                cmd->writeBuffer(position_vb, position.data(), sizeof(glm::vec3) * position.size(), 0);
                cmd->writeBuffer(indices_vb, indices.data(), sizeof(uint32_t) * indices.size(), 0);
                if(uv->size() == vertexCount) {
                    uvs_vb = device->createBuffer(alina::BufferDesc().setDebugName("MESH - UV VB").setType(alina::BufferType::VERTEX));
                    cmd->writeBuffer(uvs_vb, uv->data(), sizeof(glm::vec2) * uv->size(), 0);
                }
                if(normals.size() == vertexCount) {
                    normals_vb = device->createBuffer(alina::BufferDesc().setDebugName("MESH - Normal VB").setType(alina::BufferType::VERTEX));
                    cmd->writeBuffer(normals_vb, normals.data(), sizeof(glm::vec3) * normals.size(), 0);
                }
                cmd->end();
                device->execute(cmd);
            }
        };
        Mesh() : mData(CreateRef<Data>()) {}
        inline Mesh clone() {return Mesh(mData); };
        inline void SetPosition(std::vector<glm::vec3> data) {mData->position = data;}
        inline void SetUvs(std::vector<glm::vec2> data) {mData->uv = data;}
        inline void SetNormals(std::vector<glm::vec3> data) {mData->normals = data;}
        inline void SetIndices(std::vector<uint32_t> data) {mData->indices = data;}
        inline std::vector<glm::vec3>& GetPosition() {return mData->position;}
        inline std::vector<glm::vec2>& GetUvs() {return mData->uv;}
        inline std::vector<glm::vec3>& GetNormals() {return mData->normals;}
        inline std::vector<uint32_t>& GetIndices() {return mData->indices;}
        inline Data* GetData() {return mData.get();};
        inline void ChangedData() {mNeedChange = true;}
        inline bool NeedChange() {return mNeedChange;}
        inline void SetNeedChange(bool value) {mNeedChange = value;}
    private:
        Mesh(Ref<Data>& data){
            mData = CreateRef<Data>();
            mData->position = data->position;
            mData->uv = data->uv;
            mData->normals = data->normals;
        }

        Ref<Data> mData; 
        bool mNeedChange = false;
    };
}