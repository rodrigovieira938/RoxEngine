#include <RoxEngine/renderer/slang/slang.hpp>
#include "alina/alina.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include <RoxEngine/renderer/Material.hpp>
#include <format>
#include <functional>
#include <unordered_map>
#include <RoxEngine/core/Logger.hpp>
#include <RoxEngine/core/Engine.hpp>

namespace RoxEngine {
    Material::Material(LoadedMaterial& loadedMaterial, const std::string& variant) 
    {
        auto& loadedVariant = loadedMaterial.getVariantByName(variant);
        
        auto& target = loadedMaterial.getTargetByFormat(loadedVariant, SLANG_SPIRV);

        mResources = target.reflection;

        auto& vertex_code = target.entryPoints[0].code;
        auto& fragment_code = target.entryPoints[1].code;
        mDevice = Engine::Get()->GetWindow()->GetDevice().get();
        mVertexShader = mDevice->createShader(alina::ShaderType::VERTEX, vertex_code.data(), vertex_code.size());
        mFragmentShader = mDevice->createShader(alina::ShaderType::FRAGMENT, fragment_code.data(), fragment_code.size()); 

        auto cmd = mDevice->createCommandList();
        cmd->begin();
        for(auto& resource : mResources) {
            switch(resource.kind) {
            case ResourceKind::ConstantBuffer: {
                auto ubo = mDevice->createBuffer(alina::BufferDesc().setType(alina::BufferType::UNIFORM));
                cmd->writeBuffer(ubo, nullptr, resource.layout->size, 0);
                mUbos.push_back(Ubo{ubo, resource.bindingIndex, resource.bindingSpace});
                mShaderResources.uboBinding.push_back(
                    alina::UniformBufferBinding()
                        .setBuffer(ubo)
                        .setSet(resource.bindingSpace)
                        .setBinding(resource.bindingIndex)
                );
                break;
            }
            case ResourceKind::StructuredBuffer:
            case ResourceKind::Texture:
            case ResourceKind::Sampler:
            case ResourceKind::Other:
                log::fatal("Unhandled kind at Material.cpp 35");
              break;
            }
        }
        cmd->end();
        mDevice->execute(cmd);
    }
    alina::GraphicsPipeline Material::GetGraphicsPipeline(AlinaGlue::InputLayoutPool& inputLayoutPool, AlinaGlue::GraphicsPipelinePool& pipelinePool) {
        if(mGraphicsPipeline) {
            return mGraphicsPipeline;
        }

        std::vector<alina::VertexAttributeDesc> inputlayout_desc;
        inputlayout_desc.resize(1);
        inputlayout_desc[0] = alina::VertexAttributeDesc().setFormat(alina::VertexAttributeFormat::Float).setArraySize(3).setStride(sizeof(float)*3);
        
        /*inputlayout_desc.reserve(mModuleReflection->vertex_inputs.size());
        for(auto& vertex_input : mModuleReflection->vertex_inputs) {
            uint32_t arraySize, format_size = sizeof(float);
            auto format = alina::VertexAttributeFormat::Float;
            switch(vertex_input) {
                case ShaderReflection::VertexBindingPoint::POSITION:
                case ShaderReflection::VertexBindingPoint::NORMAL:
                    arraySize = 3;
                    break;
                case ShaderReflection::VertexBindingPoint::UV0:
                    arraySize = 2;
                    break;
                case ShaderReflection::VertexBindingPoint::UV1:
                case ShaderReflection::VertexBindingPoint::TANGENT:
                case ShaderReflection::VertexBindingPoint::BITANGENT:
                case ShaderReflection::VertexBindingPoint::COLOR0:
                    throw std::runtime_error(std::format("TODO: support {} vertex binding point!", vertex_input.getName()));
            }
            inputlayout_desc.emplace_back(alina::VertexAttributeDesc().setFormat(format).setArraySize(arraySize).setStride(format_size*arraySize));
        }
        if(mModuleReflection->instance_data.has_value()) {
                static std::function<void(const ShaderReflection::Type *type)> addAttrs;
                addAttrs = [&](const ShaderReflection::Type *type){
                    //TODO: use frozen unordered_map
                    static std::unordered_map<std::string_view, alina::VertexAttributeFormat> typeNameToFormat = {
                        {"float", alina::VertexAttributeFormat::Float},
                        {"int", alina::VertexAttributeFormat::Int}
                    };
                    const ShaderReflection::Type* innerType = type;
                    
                    if(type->kind != ShaderReflection::Type::Scalar) {
                        while(innerType->innerType != nullptr) {
                            innerType = innerType->innerType;
                        }
                    }
                    if(!typeNameToFormat.contains(innerType->name)) {
                        throw std::runtime_error(std::format("Unknowed scalar type {}", innerType->name));
                    }
                    alina::VertexAttributeFormat format = typeNameToFormat.at(innerType->name);

                    if(type->kind == ShaderReflection::Type::Struct)
                        //InstanceData doesnt support structs, so they should be flattened down
                        throw std::runtime_error("");
                    
                    uint32_t arraySize = 1;
                    switch (type->kind) {
                    case ShaderReflection::Type::Scalar:
                        break;
                    case ShaderReflection::Type::Vector:
                    case ShaderReflection::Type::Array:
                        arraySize = type->rowCount;
                        break;
                    case ShaderReflection::Type::Matrix:
                        arraySize = type->rowCount * type->colCount;
                        break;
                    case ShaderReflection::Type::Struct:
                    case ShaderReflection::Type::Unknown:
                        throw std::runtime_error(""); //Unexpected state
                      break;
                    }
                    inputlayout_desc.push_back(alina::VertexAttributeDesc().setFormat(format)
                        .setBufferIndex(mModuleReflection->vertex_inputs.size())
                        .setArraySize(arraySize).setStride(type->size)
                        .setInstanced(true));
                };
            addAttrs(mModuleReflection->instance_data->type);
        }*/
        auto pipeline_desc = alina::GraphicsPipelineDesc()
            .setVertexShader(mVertexShader)
            .setFragmentShader(mFragmentShader)
            .setInputLayout(inputLayoutPool.Get(inputlayout_desc));
        
        mGraphicsPipeline = pipelinePool.Get(pipeline_desc);

        return mGraphicsPipeline;
    }
    std::optional<Material::ParsedPath> Material::parsePath(std::string_view path)
    {
        ParsedPath result;

        while (!path.empty())
        {
            auto dot = path.find('.');

            std::string_view segment =
                path.substr(0, dot);

            if (segment.empty())
                return std::nullopt;

            if (dot == std::string_view::npos)
                path.remove_prefix(path.size());
            else
                path.remove_prefix(dot + 1);

            // Field name.
            auto bracket = segment.find('[');

            std::string_view name =
                segment.substr(0, bracket);

            if (name.empty())
                return std::nullopt;

            result.elements.push_back({
                PathElementKind::Field,
                name,
                0
            });

            // Indices.
            if (bracket == std::string_view::npos)
                continue;

            std::string_view indices =
                segment.substr(bracket);

            while (!indices.empty())
            {
                if (indices.front() != '[')
                    return std::nullopt;

                indices.remove_prefix(1);

                size_t digits = 0;
                uint32_t index = 0;

                while (digits < indices.size())
                {
                    char c = indices[digits];

                    if (c < '0' || c > '9')
                        break;

                    index = index * 10 + (c - '0');
                    ++digits;
                }

                if (digits == 0 ||
                    digits >= indices.size() ||
                    indices[digits] != ']')
                {
                    return std::nullopt;
                }

                result.elements.push_back({
                    PathElementKind::Index,
                    {},
                    index
                });

                indices.remove_prefix(digits + 1);
            }
        }

        return result;
    }
    std::optional<Material::LookupResult> Material::lookup(const Material::ParsedPath& path) {
        if (path.elements.empty())
            return std::nullopt;

        auto it = path.elements.begin();

        // First element must select a top-level resource by name.
        if (it->kind != PathElementKind::Field)
            return std::nullopt;

        const ReflectedResource* resource = nullptr;
        for (auto& r : mResources)
        {
            if (r.name == it->name)
            {
                resource = &r;
                break;
            }
        }
        //Fallback for unnamed resources
        if (!resource)
        {
            for (auto& r : mResources)
            {
                if (!r.name.empty() || !r.layout)
                    continue;

                bool hasField = false;
                for (auto& f : r.layout->fields)
                {
                    if (f.name == it->name)
                    {
                        hasField = true;
                        break;
                    }
                }

                if (hasField)
                {
                    resource = &r;
                    break;
                }
            }
        }

        if (!resource)
            return std::nullopt;

        ++it;

        LookupResult result;
        result.resource = resource;

        // Resources with no layout (Texture, Sampler, ...) can't be descended into.
        if (!resource->layout)
        {
            if (it != path.elements.end())
                return std::nullopt;

            return result;
        }

        const StructInfo* currentStruct = resource->layout.get();

        while (it != path.elements.end())
        {
            // We can only descend into a struct via a field name.
            if (it->kind != PathElementKind::Field || !currentStruct)
                return std::nullopt;

            const FieldNode* field = nullptr;
            for (auto& f : currentStruct->fields)
            {
                if (f.name == it->name)
                {
                    field = &f;
                    break;
                }
            }

            if (!field)
                return std::nullopt;

            result.field = field;
            result.offset += field->offset;
            result.scalar = nullptr;
            currentStruct = nullptr;

            ++it;

            if (auto* scalar = std::get_if<ScalarInfo>(&field->data))
            {
                result.scalar = scalar;

                // Optional trailing index into this field, if it's an array.
                if (it != path.elements.end() && it->kind == PathElementKind::Index)
                {
                    if (scalar->arrayElementCount == 0)
                        return std::nullopt; // not an array

                    if (it->index >= scalar->arrayElementCount)
                        return std::nullopt; // out of bounds

                    result.offset += static_cast<size_t>(it->index) * scalar->arrayStride;

                    ++it;

                    // Only 1D arrays are supported right now; a second consecutive
                    // Index element here means the path over-indexed. Falling
                    // through, the next loop iteration will see kind == Index
                    // (not Field) and correctly reject it.
                }
            }
            else
            {
                // Nested struct.
                auto& nested = std::get<std::shared_ptr<StructInfo>>(field->data);
                currentStruct = nested.get();

                // Array-of-struct isn't implemented yet, matching parsePath's
                // 1D-array limitation and the known gap in the extractor.
                if (it != path.elements.end() && it->kind == PathElementKind::Index)
                    return std::nullopt;
            }
        }

        return result;
    }
    bool Material::Set(std::string_view path, std::string_view type, const void* data, size_t size) {
        auto paths = parsePath(path);
        if(!paths.has_value())
            return false;
        auto lookupResult = lookup(*paths);
        if(!lookupResult.has_value())
            return false;
        for(auto& ubo : mUbos) {
            if(ubo.bindingIndex == lookupResult->resource->bindingIndex && ubo.bindingSpace == lookupResult->resource->bindingSpace) {
                auto cmd = mDevice->createCommandList();
                cmd->begin();
                cmd->writeBuffer(ubo.buffer, data, size, lookupResult->offset);
                cmd->end();
                mDevice->execute(cmd);
                return true;
            }
        }
        return false;
    }
}