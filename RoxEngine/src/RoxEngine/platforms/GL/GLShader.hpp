#pragma once
#include "RoxEngine/renderer/Shader.hpp"
#include <RoxEngine/platforms/GL/GLUniformBuffer.hpp>
#include <cstdint>
#include <unordered_map>
#include "include/slang-com-ptr.h"
namespace slang
{
	struct IModule;
}

namespace RoxEngine::GL {
    class Shader final : public ::RoxEngine::Shader {
    public:
        //TODO: add preprocessor support
        Shader(const std::string& src, const std::string& module_name);
        Shader(const std::string& path);
    	virtual ~Shader() final = default;

        Slang::ComPtr<slang::IModule> mModule;
    };
}
