module;
#include <string>
export module roxengine:renderer_shader;
import :utils;
export namespace RoxEngine {
    class Shader {
    public:
    	static Ref<Shader> Create(const std::string& src, const std::string& module_name);
        static Ref<Shader> Create(const std::string& path);
    };
}