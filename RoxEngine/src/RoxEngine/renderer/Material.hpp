#pragma once
#include "RoxEngine/slang/slang.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/utils/Utils.hpp>
#include <glm/glm.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>

namespace RoxEngine {
    class Material {
    public:
        Material(alina::Shader vertex_shader, alina::Shader fragment_shader, Ref<ModuleReflection> moduleReflection);
        inline bool Set(std::string_view path, float value) { return Set(path, "float", &value, sizeof(float));}
        inline bool Set(std::string_view path, glm::vec2 value) { return Set(path, "vector<float,2>", &value.x, sizeof(glm::vec3));}
        inline bool Set(std::string_view path, glm::vec3 value) { return Set(path, "vector<float,3>", &value.x, sizeof(glm::vec3));}
        inline bool Set(std::string_view path, glm::vec4 value) { return Set(path, "vector<float,4>", &value.x, sizeof(glm::vec4));}
        inline bool Set(std::string_view path, glm::mat3 value) { return Set(path, "matrix<float,3,3>", &value[0][0], sizeof(glm::mat3));}
        inline bool Set(std::string_view path, glm::mat4 value) { return Set(path, "matrix<float,4,4>", &value[0][0], sizeof(glm::mat4));}
        inline alina::Shader GetVertexShader() {return mVertexShader;}    
        inline alina::Shader GetFragmentShader() {return mFragmentShader;}
        inline alina::ShaderResources& GetShaderResources() {return mShaderResources;}
        inline Ref<ModuleReflection> GetModuleReflection() {return mModuleReflection;}
    private:
        bool Set(std::string_view path, std::string_view type, const void* data, size_t size);
        alina::IDevice* mDevice;
        alina::Shader mVertexShader, mFragmentShader;
        Ref<ModuleReflection> mModuleReflection;
        std::vector<alina::Buffer> mUbos;
        alina::ShaderResources mShaderResources;
    };
}