#pragma once
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <RoxEngine/utils/Utils.hpp>
#include "slang-com-ptr.h"
#include "slang.h"
#include <RoxEngine/renderer/ShaderReflection.hpp>

namespace RoxEngine {
    struct ModuleReflection {
        struct Field {
            std::string name;
            size_t offset;
            const ShaderReflection::Type* type = nullptr;
        };
        struct UniformBuffer {
            std::string name;
            size_t size;
            std::vector<Field> fields;
            uint32_t binding_index, binding_space;
        };
        struct SharedUniformBuffer : public UniformBuffer{
            SharedUniformBuffer(UniformBuffer&& other){
                this->name = std::move(other.name);
                this->size = std::move(other.size);
                this->fields = std::move(other.fields);
                this->binding_index = other.binding_index;
                this->binding_space = other.binding_space;
            }
            std::string index_name;
        };
        struct LookupResult {
            size_t offset;
            size_t ubo_index;
            bool is_shared = false;
            const ShaderReflection::Type* type;
        };
        std::optional<LookupResult> lookup(std::string_view path);
        ShaderReflection::Type* types;
        size_t types_size;
        std::vector<ModuleReflection::UniformBuffer> ubos;
        std::vector<ModuleReflection::SharedUniformBuffer> shared_ubos;
        std::vector<ShaderReflection::VertexBindingPoint> vertex_inputs;
    };
    class SlangLayer
    {
    public:
        enum class EntryPointType {
            VERTEX,
            FRAGMENT
        };

        static void Init();
        static Slang::ComPtr<slang::IModule> CompileModule(const std::string& filepath);
        static Slang::ComPtr<slang::IEntryPoint> GetModuleEntryPoint(Slang::ComPtr<slang::IModule> module, EntryPointType type);
        static Slang::ComPtr<slang::IComponentType> CreateCompositeComponentType(std::span<slang::IComponentType*> components);
        static Slang::ComPtr<slang::IComponentType> LinkModule(slang::IComponentType* component);
        static std::string GetModuleCode(slang::IComponentType* component, uint32_t entryPointIndex = 0);
        static ModuleReflection GetProgramReflection(Slang::ComPtr<slang::IComponentType> program);
        static void Shutdown();
    };
};