#pragma once

#include <slang.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct StructInfo;

struct ScalarInfo
{
    size_t size = 0;
    slang::TypeReflection::ScalarType scalarType = slang::TypeReflection::ScalarType::None;
    uint32_t vectorElementCount = 0; // Vector: element count. Matrix: column count. 0 otherwise.
    uint32_t matrixRowCount = 0;     // Matrix only.
    //TODO: support multidimensional arrays, for now we only support 1D arrays
    uint32_t arrayElementCount = 0;  // 0 = not an array; otherwise element count of the outer array.
    uint32_t arrayStride = 0; // stride between array elements
    uint32_t matrixStride = 0; //stride between rows
};

struct FieldNode
{
    std::string name;
    size_t offset = 0;
    std::variant<ScalarInfo, std::shared_ptr<StructInfo>> data;
};

struct StructInfo
{
    size_t size = 0;
    std::vector<FieldNode> fields;
};

enum class ResourceKind
{
    ConstantBuffer,
    StructuredBuffer,
    Texture,
    Sampler,
    Other
};

inline const char* toString(ResourceKind k)
{
    switch (k)
    {
        case ResourceKind::ConstantBuffer:   return "constant_buffer";
        case ResourceKind::StructuredBuffer: return "structured_buffer";
        case ResourceKind::Texture:          return "texture";
        case ResourceKind::Sampler:          return "sampler";
        case ResourceKind::Other:            return "other";
    }
    return "";
}

struct ReflectedResource
{
    std::string name;
    ResourceKind kind = ResourceKind::Other;
    unsigned bindingIndex = 0;
    unsigned bindingSpace = 0;
    std::shared_ptr<StructInfo> layout;
};

namespace detail
{
    inline bool isResourceKind(slang::TypeReflection::Kind kind)
    {
        return kind == slang::TypeReflection::Kind::Resource ||
               kind == slang::TypeReflection::Kind::SamplerState ||
               kind == slang::TypeReflection::Kind::TextureBuffer ||
               kind == slang::TypeReflection::Kind::ConstantBuffer ||
               kind == slang::TypeReflection::Kind::ParameterBlock;
    }

    inline ResourceKind mapResourceKind(slang::TypeLayoutReflection* typeLayout)
    {
        slang::TypeReflection::Kind kind = typeLayout->getKind();
        if (kind == slang::TypeReflection::Kind::SamplerState) return ResourceKind::Sampler;
        if (kind == slang::TypeReflection::Kind::ConstantBuffer ||
            kind == slang::TypeReflection::Kind::ParameterBlock)
            return ResourceKind::ConstantBuffer;
        if (kind == slang::TypeReflection::Kind::Resource)
        {
            SlangResourceShape shape = typeLayout->getResourceShape();
            SlangResourceShape baseShape = static_cast<SlangResourceShape>(shape & SLANG_RESOURCE_BASE_SHAPE_MASK);
            if (baseShape == SLANG_STRUCTURED_BUFFER) return ResourceKind::StructuredBuffer;
            return ResourceKind::Texture; // default assumption for other resource shapes
        }
        return ResourceKind::Other;
    }

    inline ScalarInfo reflectScalarFamily(slang::TypeLayoutReflection* typeLayout)
    {
        ScalarInfo info;
        slang::TypeReflection::Kind kind = typeLayout->getKind();

        info.size = typeLayout->getSize(slang::ParameterCategory::Uniform);

        if (kind == slang::TypeReflection::Kind::Array)
        {
            info.arrayElementCount = static_cast<uint32_t>(typeLayout->getElementCount());
            info.arrayStride = typeLayout->getElementStride(SLANG_PARAMETER_CATEGORY_UNIFORM);
            typeLayout = typeLayout->getElementTypeLayout(); // unwrap for shape info only
            kind = typeLayout->getKind();
        }

        if (kind == slang::TypeReflection::Kind::Scalar)
        {
            info.scalarType = typeLayout->getType()->getScalarType();
        }
        else if (kind == slang::TypeReflection::Kind::Vector)
        {
            info.scalarType = typeLayout->getType()->getScalarType();
            info.vectorElementCount = static_cast<uint32_t>(typeLayout->getType()->getElementCount());
        }
        else if (kind == slang::TypeReflection::Kind::Matrix)
        {
            info.scalarType = typeLayout->getType()->getScalarType();
            info.matrixRowCount = typeLayout->getType()->getRowCount();
            info.vectorElementCount = typeLayout->getType()->getColumnCount();
            info.matrixStride = typeLayout->getElementTypeLayout()->getStride();
        }

        return info;
    }

    struct ExtractionContext
    {
        std::vector<ReflectedResource> resources;
        std::unordered_map<slang::TypeReflection*, std::shared_ptr<StructInfo>> structCache;
    };

    inline std::shared_ptr<StructInfo> reflectStruct(slang::TypeLayoutReflection* structTypeLayout,
                                                       ExtractionContext& ctx);

    inline ReflectedResource reflectResource(const std::string& name,
                                              slang::TypeLayoutReflection* typeLayout,
                                              unsigned bindingIndex,
                                              unsigned bindingSpace,
                                              ExtractionContext& ctx)
    {
        ReflectedResource res;
        res.name = name;
        res.kind = mapResourceKind(typeLayout);
        res.bindingIndex = bindingIndex;
        res.bindingSpace = bindingSpace;

        if (res.kind == ResourceKind::ConstantBuffer)
        {
            slang::TypeLayoutReflection* elementLayout = typeLayout->getElementTypeLayout();
            if (elementLayout && elementLayout->getKind() == slang::TypeReflection::Kind::Struct)
                res.layout = reflectStruct(elementLayout, ctx);
        }

        return res;
    }

    inline std::shared_ptr<StructInfo> reflectStruct(slang::TypeLayoutReflection* structTypeLayout,
                                                       ExtractionContext& ctx)
    {
        slang::TypeReflection* identity = structTypeLayout->getType();

        auto cached = ctx.structCache.find(identity);
        if (cached != ctx.structCache.end())
            return cached->second; // reuse - this is the actual dedup

        auto info = std::make_shared<StructInfo>();
        ctx.structCache[identity] = info;

        info->size = structTypeLayout->getSize(slang::ParameterCategory::Uniform);

        unsigned fieldCount = structTypeLayout->getFieldCount();
        info->fields.reserve(fieldCount);
        for (unsigned i = 0; i < fieldCount; ++i)
        {
            slang::VariableLayoutReflection* fieldLayout = structTypeLayout->getFieldByIndex(i);
            if (!fieldLayout) continue;

            slang::TypeLayoutReflection* fieldTypeLayout = fieldLayout->getTypeLayout();
            slang::TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();

            if (isResourceKind(fieldKind))
            {
                std::string fieldName = fieldLayout->getName() ? fieldLayout->getName() : "";
                ctx.resources.push_back(reflectResource(
                    fieldName, fieldTypeLayout,
                    fieldLayout->getBindingIndex(), fieldLayout->getBindingSpace(), ctx));
                continue;
            }

            FieldNode node;
            node.name = fieldLayout->getName() ? fieldLayout->getName() : "";
            node.offset = fieldLayout->getOffset(slang::ParameterCategory::Uniform);

            if (fieldKind == slang::TypeReflection::Kind::Struct)
                node.data = reflectStruct(fieldTypeLayout, ctx);
            else
                node.data = reflectScalarFamily(fieldTypeLayout);

            info->fields.push_back(std::move(node));
        }

        return info;
    }
}

inline std::vector<ReflectedResource> extractReflection(slang::ProgramLayout* layout)
{
    detail::ExtractionContext ctx;
    if (!layout) return ctx.resources;

    unsigned paramCount = layout->getParameterCount();
    for (unsigned i = 0; i < paramCount; ++i)
    {
        slang::VariableLayoutReflection* param = layout->getParameterByIndex(i);
        if (!param) continue;

        slang::TypeLayoutReflection* typeLayout = param->getTypeLayout();
        slang::TypeReflection::Kind kind = typeLayout->getKind();

        if (kind == slang::TypeReflection::Kind::ParameterBlock ||
            kind == slang::TypeReflection::Kind::ConstantBuffer ||
            detail::isResourceKind(kind))
        {
            std::string name = param->getName() ? param->getName() : "";
            ctx.resources.push_back(detail::reflectResource(
                name, typeLayout, param->getBindingIndex(), param->getBindingSpace(), ctx));
        }
    }

    return std::move(ctx.resources);
}