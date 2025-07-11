#include <glad/gl.h>
#include <memory>
#include <cassert>
#include <RoxEngine/platforms/GL/GLVertexArray.hpp>
#include <RoxEngine/platforms/GL/GLBuffer.hpp>
namespace RoxEngine::GL {
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
            default: assert(false && "Unknown ShaderDataType!");
		}
		return 0;
	}


    VertexArray::VertexArray() {
        glCreateVertexArrays(1,&mID);
    }
    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &mID);
    }
    void VertexArray::AddVertexBuffer(const std::shared_ptr<::RoxEngine::VertexBuffer>& vb) {
        glBindVertexArray(mID);
		auto glvb = std::static_pointer_cast<VertexBuffer>(vb);
        glBindBuffer(GL_ARRAY_BUFFER, glvb->mID);

		const auto& layout = glvb->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(mVBIndex);
					glVertexAttribPointer(mVBIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset);
					mVBIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(mVBIndex);
					glVertexAttribIPointer(mVBIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						layout.GetStride(),
						(const void*)element.Offset);
					mVBIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(mVBIndex);
						glVertexAttribPointer(mVBIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.Offset + sizeof(float) * count * i));
						glVertexAttribDivisor(mVBIndex, 1);
						mVBIndex++;
					}
					break;
				}
				default: assert(false && "Unknown ShaderDataType!");
			}
		}

		mVBs.push_back(vb);
    }
    void VertexArray::SetIndexBuffer(const std::shared_ptr<::RoxEngine::IndexBuffer>& indexBuffer) {
        mIB = indexBuffer;
    }
    
    const std::vector<std::shared_ptr<::RoxEngine::VertexBuffer>>& VertexArray::GetVertexBuffers() const {
        return mVBs;
    }
    const std::shared_ptr<::RoxEngine::IndexBuffer>& VertexArray::GetIndexBuffer() const {
        return mIB;
    }
}