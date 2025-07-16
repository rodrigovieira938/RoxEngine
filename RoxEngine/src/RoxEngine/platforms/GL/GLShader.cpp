#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/filesystem/Filesystem.hpp"
#include <glad/gl.h>
#include <iostream>
#include <slang.h>
#include <slang-com-ptr.h>
#include <RoxEngine/platforms/GL/GLShader.hpp>
#include <RoxEngine/slang/slang.hpp>

namespace RoxEngine::GL {
    Shader::Shader(const std::string& src, const std::string& module_name)
    {
        SlangLayer::Init();
        Slang::ComPtr<slang::IBlob> diagnostics;

    	mModule = SlangLayer::sSession->loadModuleFromSourceString(module_name.c_str(), "", src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            log::error("Shader loading failed: {}", (char*)diagnostics->getBufferPointer());
            return;
        }
    }

    Shader::Shader(const std::string& path)
    {
        SlangLayer::Init();
        std::string src = FileSystem::ReadTextFile(path);

        Slang::ComPtr<slang::IBlob> diagnostics;
        std::string module_name = FileSystem::GetFileName(path,false);
        mModule = SlangLayer::sSession->loadModuleFromSourceString(module_name.c_str(), path.c_str(), src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            log::error("Shader loading failed: {}", diagnostics->getBufferPointer());
            return;
        }
    }
}
