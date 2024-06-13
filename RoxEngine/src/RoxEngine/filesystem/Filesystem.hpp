#pragma once
#include <RoxEngine/core/Buffer.h>

namespace RoxEngine::FileSystem {
	static bool Exists(const std::string& path);
	
	static size_t GetFileSize();
	static Buffer ReadFile(const std::string& path);
	static std::string ReadTextFile(const std::string& path);
}