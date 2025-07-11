#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
namespace RoxEngine {
	class UniformBuffer {
	public:
		virtual bool Set(const std::string& name, uint32_t value) = 0;
		virtual bool Set(const std::string& name, double value) = 0;
		virtual bool Set(const std::string& name, void* data, size_t size) = 0;
		//Only for internal use
		struct DataType {
			enum Kind {
				INT8,
				INT16,
				INT32,

				UINT8,
				UINT16,
				UINT32,

				HALF,
				FLOAT,
				DOUBLE,

				ARRAY,
				MATRIX,
				VECTOR,
				STRUCT,
			};
			size_t GetSize() const;
			static size_t GetSize(Kind kind);
			bool IsScalar() const;
			bool IsInteger() const;
			bool IsFloatingPoint() const;

			Kind kind;
			size_t offset, alignment = 0;
			size_t item_offset = 0; // offset between items in arrays already aligned
			Kind innerKind;		  //For vectors, arrays and matrix
			uint32_t rowsize = 1; //For vectors, arrays and matrix
			uint32_t colsize = 1; //For matrix
			size_t size; // For structs
		};
	protected:
		std::unordered_map<std::string, DataType> mDesc;
	};
}
