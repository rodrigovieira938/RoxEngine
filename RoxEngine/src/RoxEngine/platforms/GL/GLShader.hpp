#pragma once
#include <slang/include/slang.h>
#include <slang/include/slang-com-ptr.h>
#include <string>
#include <RoxEngine/renderer/Shader.hpp>

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
