#include <RoxEngine/renderer/Material.hpp>

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