#pragma once
#include <RoxEngine/renderer/Material.hpp>

namespace RoxEngine::GL {
	class Material final : public ::RoxEngine::Material {
	public:
		Material(const Ref<::RoxEngine::Shader>& shader, const EntryPointInfo& info, const char* link_time_constants = "");
		~Material() override = default;
		virtual std::vector<Ref<::RoxEngine::UniformBuffer>>& GetUbos() override;
		virtual Ref<::RoxEngine::UniformBuffer> GetUbo(const std::string& name) override;
		virtual Ref<::RoxEngine::Shader> GetShader() override;
		//virtual Ref<UniformBuffer> GetUbo(const std::string& name) override;
		//virtual Ref<::RoxEngine::Shader> GetShader() override;

		std::vector<Ref<::RoxEngine::UniformBuffer>> mUbos;
		std::unordered_map<std::string, size_t> mUboMap;

		Ref<::RoxEngine::Shader> mShader;
		uint32_t mID;
	};
}