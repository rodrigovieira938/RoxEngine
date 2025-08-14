#pragma once
#include "RoxEngine/slang/slang.hpp"
#include "alina/alina.hpp"
#include <RoxEngine/utils/Utils.hpp>
#include <glm/glm.hpp>
#include <RoxEngine/renderer/alina/ResourcePool.hpp>

namespace RoxEngine {
    class Material {
    public:
        Material(alina::Shader vertex_shader, alina::Shader fragment_shader, Ref<ModuleReflection> moduleReflection) {mVertexShader = vertex_shader; mFragmentShader = fragment_shader; mModuleReflection = moduleReflection;}
        inline alina::Shader GetVertexShader() {return mVertexShader;}    
        inline alina::Shader GetFragmentShader() {return mFragmentShader;}    
    private:
        alina::Shader mVertexShader, mFragmentShader;
        Ref<ModuleReflection> mModuleReflection;
    };
}