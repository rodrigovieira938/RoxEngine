module;
#include <RoxEngine/profiler/profiler.hpp>
module roxengine;

namespace RoxEngine{
	Ref<Material> Material::Create(const Ref<Shader>& shader, const EntryPointInfo& info, const char* link_time_constants)
	{
        PROFILER_FUNCTION();
        switch (GraphicsContext::GetAPI()) {
        case RendererApi::OPENGL:
            return CreateRef<GL::Material>(shader, info, link_time_constants);
        default:
            //TODO: Assert
            break;
        }
        return nullptr;
	}
}
