#include "GLUniformBuffer.hpp"

#include "glad/gld.h"

namespace RoxEngine::GL {
	UniformBuffer::UniformBuffer(size_t size, uint32_t binding, std::unordered_map<std::string, DataType>&& desc)
		: mSize(size) 
	{
		mDesc = std::move(desc);
		mData = calloc(1,mSize);
		mBinding = binding;
		mChanged = false;
		glGenBuffers(1, &mId);
		glBindBuffer(GL_UNIFORM_BUFFER, mId);
		glBufferData(GL_UNIFORM_BUFFER, static_cast<int64_t>(size), mData, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &mId);
		free(mData);
	}

	//only works with integers, doesn't create any new object 
	void* UniformBuffer::cast(void* ptr,size_t size, UniformBuffer::DataType datatype)
	{
		//is the same size (int32_t or uint32_t) or is smaller
		if (size == datatype.GetSize())
			return ptr;
		return static_cast<uint8_t*>(ptr) + (size - datatype.GetSize());
	}

	bool UniformBuffer::Set(const std::string& name, uint32_t value)
	{
		auto it = mDesc.find(name);
		if (it == mDesc.end()) return false;
		auto& elem = it->second;
		//TODO: handle arrays, matrices and vectors
		if(!elem.IsScalar())
		{
			return false;
		}
		if(elem.IsInteger())
		{
			std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, cast(&value, sizeof(uint32_t), elem), elem.GetSize());
		}
		else
		{
			switch (elem.kind)
			{
				case DataType::HALF: return false;
				case DataType::FLOAT:
				{
					const float val = static_cast<float>(value);
					std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, &val, sizeof(float));
					break;
				}
				case DataType::DOUBLE:
				{
					const double val = static_cast<double>(value);
					std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, &val, sizeof(double));
					break;
				}
			}
		}
		mChanged = true;
		return true;
	}

	bool UniformBuffer::Set(const std::string& name, double value)
	{
		auto it = mDesc.find(name);
		if (it == mDesc.end()) return false;
		auto& elem = it->second;
		if (!elem.IsScalar())
		{
			return false;
		}
		if(elem.IsInteger())
		{
			return Set(name, static_cast<uint32_t>(value));
		}
		switch (elem.kind)
		{
			case DataType::HALF:
				return false;
			case DataType::FLOAT: {
				float val = static_cast<float>(value);
				std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, &val, sizeof(float));
				break;
			}
			case DataType::DOUBLE: {
				std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, &value, sizeof(float));
				break;
			}
		}


		mChanged = true;
		return true;
	}

	bool UniformBuffer::Set(const std::string& name, void* data, size_t size)
	{
		mSize;
		auto it = mDesc.find(name);
		if (it == mDesc.end()) return false;
		auto& elem = it->second;
		if (size != elem.GetSize())
			return false;
		std::memcpy(static_cast<uint8_t*>(mData) + elem.offset, data, size);
		mChanged = true;
		return true;
	}

	void UniformBuffer::update()
	{
		if (!mChanged) return;
		glBindBuffer(GL_UNIFORM_BUFFER, mId);
		glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(mSize), mData, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
