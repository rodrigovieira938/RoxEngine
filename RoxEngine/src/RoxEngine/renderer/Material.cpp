#include <RoxEngine/renderer/slang/slang.hpp>
#include "alina/alina.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include <RoxEngine/renderer/Material.hpp>
#include <format>
#include <functional>
#include <unordered_map>
#include <RoxEngine/core/Logger.hpp>

namespace RoxEngine {
    Material::Material(alina::Shader vertex_shader, alina::Shader fragment_shader, Ref<ModuleReflection> moduleReflection) {
        mDevice = vertex_shader->getDevice();
        mVertexShader = vertex_shader; 
        mFragmentShader = fragment_shader; 
        mModuleReflection = moduleReflection;
        mUbos.reserve(moduleReflection->ubos.size()+moduleReflection->instance_data.has_value());
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
        if(moduleReflection->instance_data.has_value()) {
            auto vbo = device->createBuffer(alina::BufferDesc().setType(alina::BufferType::VERTEX));
            cmd->writeBuffer(vbo, nullptr, moduleReflection->instance_data->type->size, 0);
            mUbos.push_back(vbo);
        }
        for(auto& reflection_ubo : moduleReflection->shared_ubos) {
            //TODO: get the ubo from some sort of global list
            mShaderResources.uboBinding.push_back(alina::UniformBufferBinding().setBuffer(nullptr).setBinding(reflection_ubo.binding_index).setSet(reflection_ubo.binding_space));
        }
        cmd->end();
        device->execute(cmd);
    }
    alina::GraphicsPipeline Material::GetGraphicsPipeline(AlinaGlue::InputLayoutPool& inputLayoutPool, AlinaGlue::GraphicsPipelinePool& pipelinePool) {
        if(mGraphicsPipeline) {
            return mGraphicsPipeline;
        }

        std::vector<alina::VertexAttributeDesc> inputlayout_desc;
        inputlayout_desc.reserve(mModuleReflection->vertex_inputs.size());
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
        }
        auto pipeline_desc = alina::GraphicsPipelineDesc()
            .setVertexShader(mVertexShader)
            .setFragmentShader(mFragmentShader)
            .setInputLayout(inputLayoutPool.Get(inputlayout_desc));
        
        mGraphicsPipeline = pipelinePool.Get(pipeline_desc);

        return mGraphicsPipeline;
    }
    bool Material::Set(std::string_view path, std::string_view type, const void* data, size_t size) {
        auto lookupResult = mModuleReflection->lookup(path);
        if(!lookupResult)
            return false;
        if(lookupResult->type->name != type)
            return false;
        auto ubo_index = lookupResult->ubo_index;
        if(lookupResult->is_instancedata) {
            ubo_index = mUbos.size()-1;
        }
        auto ubo = mUbos[ubo_index];
        auto cmd = mDevice->createCommandList();
        cmd->begin();
        cmd->writeBuffer(ubo, data, size, lookupResult->offset);
        cmd->end();
        mDevice->execute(cmd);
        return true;
    }
}