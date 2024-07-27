#pragma once
#include <RoxEngine/renderer/Shader.hpp>
#include <RoxEngine/renderer/UniformBuffer.hpp>
#include <vector>

namespace RoxEngine {
	class Material {
	public:
		virtual ~Material() = default;

		struct EntryPointInfo
		{
			const char* vertex_main;
			const char* fragment_main;
		};
		static Ref<Material> Create(const Ref<Shader>& shader, const EntryPointInfo& info, const char* link_time_constants = "");

		virtual Ref<UniformBuffer> GetUbo(const std::string& name) = 0;
		virtual std::vector<Ref<UniformBuffer>>& GetUbos() = 0;
		virtual Ref<Shader> GetShader() = 0;
	};
}
