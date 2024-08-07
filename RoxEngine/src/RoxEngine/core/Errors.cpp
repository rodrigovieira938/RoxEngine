module;
#include <string_view>
module roxengine;

namespace RoxEngine
{
	RuntimeError::RuntimeError(const std::string& error) noexcept : mMessage(error) {}
	RuntimeError::RuntimeError(const RuntimeError& other) noexcept  : mMessage(other.mMessage) {}
	RuntimeError& RuntimeError::operator=(const RuntimeError& other) noexcept { mMessage = other.mMessage; return *this; }
	std::string_view RuntimeError::GetError() { return mMessage; }
}
