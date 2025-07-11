#pragma once
#include "RoxEngine/utils/Utils.hpp"
#include <memory>
#include <string>
#include <cassert>
#include <vector>
namespace RoxEngine {
    enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	uint32_t ShaderDataTypeSize(ShaderDataType type);

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(const std::string& name, ShaderDataType type, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Mat3:    return 3; // 3* float3
				case ShaderDataType::Mat4:    return 4; // 4* float4
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Bool:    return 1;
                default: assert(false && "Unknown ShaderDataType!");
			}
            return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(std::initializer_list<BufferElement> elements)
			: mElements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return mStride; }
		const std::vector<BufferElement>& GetElements() const { return mElements; }

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			mStride = 0;
			for (auto& element : mElements)
			{
				element.Offset = offset;
				offset += element.Size;
				mStride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, size_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(size_t size);
		static Ref<VertexBuffer> Create(void* vertices, size_t size);
	};

	// Currently only support 32-bit index buffers
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(const uint32_t* data, size_t count) = 0;
		virtual size_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, size_t count);
	};
	enum class FramebufferColorTexFormat {
		R8,
		RG8,
		RGB8,
		RGBA8,
		R16,
		RG16,
		RGB16,
		RGBA16,
		R32,
		RG32,
		RGB32,
		RGBA32,
	};
	enum class FramebufferDepthTexFormat {
		D32F,
		D32FS8U,
		D24UNS8U,
	};
	class Framebuffer
	{
	public:
		static std::shared_ptr<Framebuffer> Create(uint32_t width, uint32_t height, const std::vector<FramebufferColorTexFormat>& attachments, FramebufferDepthTexFormat depthFormat);
		virtual std::pair<int,int> GetSize() = 0;
		virtual ~Framebuffer() = default;
	};
}