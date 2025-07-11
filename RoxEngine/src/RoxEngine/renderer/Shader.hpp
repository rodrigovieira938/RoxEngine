#pragma once
#include "RoxEngine/utils/Utils.hpp"
#include <string>
namespace RoxEngine {
    class Shader {
    public:
    	static Ref<Shader> Create(const std::string& src, const std::string& module_name);
        static Ref<Shader> Create(const std::string& path);
    };
}