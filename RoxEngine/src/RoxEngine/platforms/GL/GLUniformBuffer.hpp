#pragma once
#include <RoxEngine/renderer/UniformBuffer.hpp>

namespace RoxEngine::GL {
	class UniformBuffer final : public ::RoxEngine::UniformBuffer {
	public:
		UniformBuffer(size_t size,uint32_t binding, std::unordered_map<std::string, DataType>&& desc);
		virtual ~UniformBuffer();

		bool Set(const std::string& name, uint32_t value) override;
		bool Set(const std::string& name, double value) override;
		bool Set(const std::string& name, void* data, size_t size) override;

		void update();

		uint32_t mId, mBinding;
		size_t mSize;
		void* mData;
		bool mChanged;
	private:
		friend class Shader;
		void* cast(void* ptr, size_t size, DataType datatype);
	};
}
