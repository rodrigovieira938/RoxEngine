#include "RoxEngine/renderer/GraphicsContext.hpp"
#include <RoxEngine/profiler/profiler.hpp>
#include <RoxEngine/platforms/GL/GLMaterial.hpp>
#include <RoxEngine/utils/Utils.hpp>
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
