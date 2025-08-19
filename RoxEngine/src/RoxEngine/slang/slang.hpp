#pragma once
#include <optional>
#include <span>
#include <string>
#include <unordered_set>
#include <vector>
#include <RoxEngine/utils/Utils.hpp>
#include "slang-com-ptr.h"
#include "slang.h"

namespace RoxEngine {
    namespace ShaderReflection {
        struct Type {
            enum TypeKind {
                Scalar,
                Vector,
                Matrix,
                Array,
                Struct,
                Unknown
            };
            std::string name;       // e.g. "float", "vec3", "MyStruct"
            TypeKind kind = TypeKind::Unknown;

            uint32_t colCount = 0;  // columns for vector/matrix, elements for array
            uint32_t rowCount = 0; // alias for rowCount in matrices or array size

            uint32_t size = 0;
            uint32_t stride = 0;

            const Type* innerType = nullptr;  // element type for arrays, vectors, matrices

            std::vector<std::pair<std::string, const Type*>> fields; // for structs

            bool operator==(const Type& other) const {
                return kind == other.kind &&
                    name == other.name &&
                    colCount == other.colCount &&
                    rowCount == other.rowCount &&
                    size == other.size &&
                    innerType == other.innerType &&
                    fields == other.fields;
            }
        };
    };
}
namespace std {
    template <>
    struct hash<RoxEngine::ShaderReflection::Type> {
        size_t operator()(const RoxEngine::ShaderReflection::Type& t) const {
            size_t h = std::hash<int>()(static_cast<int>(t.kind));
            h ^= std::hash<std::string>()(t.name) << 1;
            h ^= std::hash<uint32_t>()(t.colCount) << 2;
            h ^= std::hash<uint32_t>()(t.rowCount) << 3;
            h ^= std::hash<uint32_t>()(t.size) << 5;
            h ^= std::hash<const RoxEngine::ShaderReflection::Type*>()(t.innerType) << 7;

            for (auto& f : t.fields) {
                h ^= std::hash<std::string>()(f.first) << 8;
                h ^= std::hash<const RoxEngine::ShaderReflection::Type*>()(f.second) << 9;
            }
            return h;
        }
    };
}
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
        };
        struct LookupResult {
            size_t offset;
            size_t ubo_index;
        };
        std::optional<LookupResult> lookup(std::string_view path);
        std::unordered_set<ShaderReflection::Type> types;
        std::vector<ModuleReflection::UniformBuffer> ubos;
    };
    class SlangLayer
    {
    public:
        static void Init();
        static Slang::ComPtr<slang::IModule> CompileModule(const std::string& filepath);
        static ModuleReflection GetModuleReflection(Slang::ComPtr<slang::IModule> module);
        static std::string LinkModules(std::span<Slang::ComPtr<slang::IModule>> modules, bool vertex_shader = true);
        static void Shutdown();
    };
};