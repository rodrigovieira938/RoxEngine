#include <cstdint>
#include <RoxEngine/renderer/UniformBuffer.hpp>

namespace RoxEngine
{
	size_t UniformBuffer::DataType::GetSize() const
	{
		switch (kind)
		{
			case ARRAY:
			{
				return item_offset * rowsize;
			}
			case MATRIX:
				return GetSize(innerKind) * rowsize * colsize;
			case VECTOR:
				return GetSize(innerKind) * rowsize;
			case STRUCT:
				return size;
			default: return GetSize(kind);
		}
	}

	size_t UniformBuffer::DataType::GetSize(Kind kind)
	{
		switch (kind)
		{
		case INT8:
			return sizeof(int8_t);
		case INT16:
			return sizeof(int16_t);
		case INT32:
			return sizeof(int32_t);
		case UINT8:
			return sizeof(uint8_t);
		case UINT16:
			return sizeof(uint16_t);
		case UINT32:
			return sizeof(uint32_t);
		case HALF:
			return sizeof(int16_t);
		case FLOAT:
			return sizeof(float);
		case DOUBLE:
			return sizeof(double);
		case ARRAY:
		case MATRIX:
		case VECTOR:
		case STRUCT:
			return 0;
		}
		return 0;
	}

	bool UniformBuffer::DataType::IsScalar() const
	{
		return IsInteger() || IsFloatingPoint();
	}

	bool UniformBuffer::DataType::IsInteger() const
	{
		return kind >= DataType::INT8 && kind <= DataType::UINT32;
	}

	bool UniformBuffer::DataType::IsFloatingPoint() const
	{
		return kind >= DataType::HALF && kind <= DataType::DOUBLE;
	}
}
