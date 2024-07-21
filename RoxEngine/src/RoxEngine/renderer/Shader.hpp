#pragma once
#include <string>
#include "RoxEngine/utils/Memory.h"

namespace RoxEngine {
    class Shader {
    public:
        struct EntryPointInfo
        {
            const char* vertex_main;
            const char* fragment_main;
        };

    	static Ref<Shader> Create(const std::string& src, const std::string& module_name, const EntryPointInfo& entry_point = { "default_vertex_main", "default_fragment_main" });
        static Ref<Shader> Create(const std::string& path, const EntryPointInfo& entry_point = {"default_vertex_main", "default_fragment_main"});
    };
}