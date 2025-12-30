#pragma once
#include <cstddef>
#include <format>
#include <limits>
#include <string>
#include <string_view>
#include <vector>
#include <RoxEngine/utils/Utils.hpp>

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

            Type* innerType = nullptr;  // element type for arrays, vectors, matrices

            std::vector<std::pair<std::string, Type*>> fields; // for structs

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
        struct VertexBindingPoint
        {
        public:
            enum Value : int
            {
                POSITION,
                UV0,
                UV1,
                NORMAL,
                TANGENT,
                BITANGENT,
                COLOR0,
                INSTANCE_DATA,
            };
        private:
            Value value;
        public:
            //The same as layout(location = x)
            uint32_t binding_index;

            constexpr VertexBindingPoint(Value v, uint32_t binding_index = 0) : value(v), binding_index(binding_index) {}

            static constexpr size_t POSITION_MAX      = 1;
            static constexpr size_t UV_MAX            = 2;
            static constexpr size_t NORMAL_MAX        = 1;
            static constexpr size_t TANGENT_MAX       = 1;
            static constexpr size_t BITANGENT_MAX     = 1;
            static constexpr size_t COLOR_MAX         = 1;
            static constexpr size_t INSTANCE_DATA_MAX = std::numeric_limits<size_t>::max();

            constexpr std::string_view getName(bool withIndex = true) const
            {
                if(withIndex) {
                    switch (value)
                    {
                        case POSITION:      return "POSITION";
                        case UV0:           return "UV0";
                        case UV1:           return "UV1";
                        case NORMAL:        return "NORMAL";
                        case TANGENT:       return "TANGENT";
                        case BITANGENT:     return "BITANGENT";
                        case COLOR0:        return "COLOR0";
                        case INSTANCE_DATA: return "INSTANCE_DATA";
                    }
                } else {
                    switch (value)
                    {
                        case POSITION:      return "POSITION";
                        case UV0:;
                        case UV1:           return "UV";
                        case NORMAL:        return "NORMAL";
                        case TANGENT:       return "TANGENT";
                        case BITANGENT:     return "BITANGENT";
                        case COLOR0:        return "COLOR";
                        case INSTANCE_DATA: return "INSTANCE_DATA";
                    }
                }
                return "";
            }

            constexpr size_t getMax() const
            {
                switch (value)
                {
                    case POSITION:      return POSITION_MAX;
                    case UV0:           return UV_MAX;
                    case UV1:           return UV_MAX;
                    case NORMAL:        return NORMAL_MAX;
                    case TANGENT:       return TANGENT_MAX;
                    case BITANGENT:     return BITANGENT_MAX;
                    case COLOR0:        return COLOR_MAX;
                    case INSTANCE_DATA: return INSTANCE_DATA_MAX;
                }
                return 0;
            }
            static constexpr VertexBindingPoint fromString(std::string_view str, size_t index = 0)
            {
                VertexBindingPoint base = VertexBindingPoint::POSITION;
                if (str == "POSITION")           base = VertexBindingPoint(POSITION);
                else if (str == "UV")            base = VertexBindingPoint(UV0);
                else if (str == "NORMAL")        base = VertexBindingPoint(NORMAL);
                else if (str == "TANGENT")       base = VertexBindingPoint(TANGENT);
                else if (str == "BITANGENT")     base = VertexBindingPoint(BITANGENT);
                else if (str == "COLOR")         base = VertexBindingPoint(COLOR0);
                else if (str == "INSTANCE_DATA") base = VertexBindingPoint(INSTANCE_DATA);
                else                             throw std::invalid_argument(std::format("Unknown BindingPoint string: \"{}\"", str));

                auto max = base.getMax();
                if(index >= max) {
                    throw std::invalid_argument(
                        std::format("Binding index {} exceeds maximum allowed for {}", 
                            index, base.getName(false))
                    );
                }
                
                return static_cast<VertexBindingPoint::Value>(static_cast<size_t>(base.value) + index);
            } 
            bool operator==(const VertexBindingPoint& other) const { return value == other.value; }
            bool operator!=(const VertexBindingPoint& other) const { return value != other.value; }

            operator Value() const {return value;}
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