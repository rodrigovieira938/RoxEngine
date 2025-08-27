#pragma once
#include "RoxEngine/slang/slang.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/utils/Utils.hpp>
#include <glm/glm.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>

namespace RoxEngine {
    class Material {
    public:
        Material(alina::Shader vertex_shader, alina::Shader fragment_shader, Ref<ModuleReflection> moduleReflection) {
            mDevice = vertex_shader->getDevice();
            mVertexShader = vertex_shader; 
            mFragmentShader = fragment_shader; 
            mModuleReflection = moduleReflection;
            mUbos.reserve(moduleReflection->ubos.size());
            mShaderResources.uboBinding.reserve(moduleReflection->ubos.size()+moduleReflection->shared_ubos.size());
            auto device = vertex_shader->getDevice();
            auto cmd = device->createCommandList();
            cmd->begin();
            for(auto& reflection_ubo : moduleReflection->ubos) {
                auto ubo = device->createBuffer(alina::BufferDesc().setType(alina::BufferType::UNIFORM));
                cmd->writeBuffer(ubo, nullptr, reflection_ubo.size, 0);
                mUbos.push_back(ubo);
                mShaderResources.uboBinding.push_back(alina::UniformBufferBinding().setBuffer(ubo).setBinding(reflection_ubo.binding_index).setSet(reflection_ubo.binding_space));
            }
            for(auto& reflection_ubo : moduleReflection->shared_ubos) {
                auto ubo = device->createBuffer(alina::BufferDesc().setType(alina::BufferType::UNIFORM));
                cmd->writeBuffer(ubo, nullptr, reflection_ubo.size, 0);
                mUbos.push_back(ubo);
                mShaderResources.uboBinding.push_back(alina::UniformBufferBinding().setBuffer(ubo).setBinding(reflection_ubo.binding_index).setSet(reflection_ubo.binding_space));
            }
            cmd->end();
            device->execute(cmd);

        }
        bool Set(std::string_view path, float value) { return Set(path, "float", &value, sizeof(float));}
        bool Set(std::string_view path, glm::vec2 value) { return Set(path, "vector<float,2>", &value.x, sizeof(glm::vec3));}
        bool Set(std::string_view path, glm::vec3 value) { return Set(path, "vector<float,3>", &value.x, sizeof(glm::vec3));}
        bool Set(std::string_view path, glm::vec4 value) { return Set(path, "vector<float,4>", &value.x, sizeof(glm::vec4));}
        bool Set(std::string_view path, glm::mat3 value) { return Set(path, "matrix<float,3,3>", &value[0][0], sizeof(glm::mat3));}
        bool Set(std::string_view path, glm::mat4 value) { return Set(path, "matrix<float,4,4>", &value[0][0], sizeof(glm::mat4));}
        inline alina::Shader GetVertexShader() {return mVertexShader;}    
        inline alina::Shader GetFragmentShader() {return mFragmentShader;}
        alina::ShaderResources& GetShaderResources() {return mShaderResources;}
    private:
        bool Set(std::string_view path, std::string_view type, const void* data, size_t size) {
            auto lookupResult = mModuleReflection->lookup(path);
            if(!lookupResult)
                return false;
            if(lookupResult->type->name != type)
                return false;
            auto ubo = mUbos[lookupResult->ubo_index];
            auto cmd = mDevice->createCommandList();
            cmd->begin();
            cmd->writeBuffer(ubo, data, size, lookupResult->offset);
            cmd->end();
            mDevice->execute(cmd);
            return true;
        }

        alina::IDevice* mDevice;
        alina::Shader mVertexShader, mFragmentShader;
        Ref<ModuleReflection> mModuleReflection;
        std::vector<alina::Buffer> mUbos;
        alina::ShaderResources mShaderResources;
    };
}