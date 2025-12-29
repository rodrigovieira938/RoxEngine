#include "RoxEngine/slang/slang.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/renderer/Material.hpp>
#include <format>

namespace RoxEngine {
    Material::Material(alina::Shader vertex_shader, alina::Shader fragment_shader, Ref<ModuleReflection> moduleReflection) {
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

        std::vector<alina::VertexAttributeDesc> inputlayout_desc(mModuleReflection->vertex_inputs.size());
        for(int i = 0; i < inputlayout_desc.size(); i++) {
            uint32_t arraySize, format_size = sizeof(float);
            auto format = alina::VertexAttributeFormat::Float;
            switch(mModuleReflection->vertex_inputs[i]) {
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
                case ShaderReflection::VertexBindingPoint::INSTANCE_DATA:
                    throw std::runtime_error(std::format("TODO: support {} vertex binding point!", mModuleReflection->vertex_inputs[i].getName()));
            }
            inputlayout_desc[i] = alina::VertexAttributeDesc().setFormat(format).setArraySize(arraySize).setStride(format_size*arraySize);
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
        auto ubo = mUbos[lookupResult->ubo_index];
        auto cmd = mDevice->createCommandList();
        cmd->begin();
        cmd->writeBuffer(ubo, data, size, lookupResult->offset);
        cmd->end();
        mDevice->execute(cmd);
        return true;
    }
}