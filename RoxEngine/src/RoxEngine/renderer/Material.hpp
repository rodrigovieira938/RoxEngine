#pragma once
#include <RoxEngine/renderer/slang/slang.hpp>
#include "alina/alina.hpp"
#include "shaderc/MaterialBinary.hpp"
#include <RoxEngine/utils/Utils.hpp>
#include <glm/glm.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>
#include <string>

namespace RoxEngine {
    class Material {
    public:
        Material(LoadedMaterial& loadedMaterial, const std::string& variant="default");
        inline bool Set(std::string_view path, float value) { return Set(path, "float", &value, sizeof(float));}
        inline bool Set(std::string_view path, glm::vec2 value) { return Set(path, "vector<float,2>", &value.x, sizeof(glm::vec3));}
        inline bool Set(std::string_view path, glm::vec3 value) { return Set(path, "vector<float,3>", &value.x, sizeof(glm::vec3));}
        inline bool Set(std::string_view path, glm::vec4 value) { return Set(path, "vector<float,4>", &value.x, sizeof(glm::vec4));}
        inline bool Set(std::string_view path, glm::mat3 value) { return Set(path, "matrix<float,3,3>", &value[0][0], sizeof(glm::mat3));}
        inline bool Set(std::string_view path, glm::mat4 value) { return Set(path, "matrix<float,4,4>", &value[0][0], sizeof(glm::mat4));}
        inline alina::Shader GetVertexShader() {return mVertexShader;}    
        inline alina::Shader GetFragmentShader() {return mFragmentShader;}
        inline alina::ShaderResources& GetShaderResources() {return mShaderResources;}
        alina::GraphicsPipeline GetGraphicsPipeline(AlinaGlue::InputLayoutPool& inputLayoutPool, AlinaGlue::GraphicsPipelinePool& pipelinePool);     
    private:
        enum class PathElementKind {
            Field,
            Index
        };
        struct PathElement
        {
            PathElementKind kind;

            std::string_view name;
            uint32_t index = 0;
        };
        struct ParsedPath
        {
            std::vector<PathElement> elements;
        };
        struct LookupResult
        {
            const ReflectedResource* resource = nullptr;
            const FieldNode* field = nullptr;
            const ScalarInfo* scalar = nullptr;

            size_t offset = 0;
        };
        struct Ubo {
            alina::Buffer buffer;
            unsigned bindingIndex = 0;
            unsigned bindingSpace = 0;
        };

        std::optional<ParsedPath> parsePath(std::string_view path);
        std::optional<LookupResult> lookup(const ParsedPath& path);
        bool Set(std::string_view path, std::string_view type, const void* data, size_t size);
  
        alina::IDevice* mDevice;
        alina::Shader mVertexShader, mFragmentShader;
        std::vector<ReflectedResource> mResources;
        std::vector<Ubo> mUbos;
        alina::ShaderResources mShaderResources;
        alina::GraphicsPipeline mGraphicsPipeline;
    };
}