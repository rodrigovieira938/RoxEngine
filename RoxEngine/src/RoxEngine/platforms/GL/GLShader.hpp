#pragma once
#include "RoxEngine/renderer/Shader.hpp"
#include <RoxEngine/platforms/GL/GLUniformBuffer.hpp>
#include <cstdint>
#include <unordered_map>

namespace RoxEngine::GL {
    class Shader final : public ::RoxEngine::Shader {
    public:
        //TODO: add preprocessor support
        Shader(const std::string& src, const std::string& module_name, const EntryPointInfo& entry_point);
        Shader(const std::string& path,const EntryPointInfo& entry_point);

    	virtual ~Shader() final;

        std::unordered_map<std::string,GL::UniformBuffer*> mUbos;
        uint32_t mID = 0;
    };
}
