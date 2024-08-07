module;
#include <glad/gl.h>
#include <iostream>
#include <slang/include/slang.h>
#include <slang/include/slang-com-ptr.h>
module roxengine;

namespace RoxEngine::GL {
    Shader::Shader(const std::string& src, const std::string& module_name)
    {
        SlangLayer::Init();
        Slang::ComPtr<slang::IBlob> diagnostics;

    	mModule = SlangLayer::sSession->loadModuleFromSourceString(module_name.c_str(), "", src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            std::cout << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
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
            std::cout << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
            return;
        }
    }
}
