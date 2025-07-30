#pragma once
#include <string>
#include "slang-com-ptr.h"
#include "slang.h"

namespace RoxEngine {
    class SlangLayer
    {
    public:
        static void Init();
        static Slang::ComPtr<slang::IModule> CompileModule(const std::string& filepath);
        static void Shutdown();
    };
};