#include "Filesystem.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace RoxEngine::FileSystem {
	bool Exists(const std::string& path)
	{
		return fs::exists(fs::path(path));
	}
	size_t GetFileSize()
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		return (size_t)file.tellg();
	}
	Buffer ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		Buffer buffer(fileSize);
		file.seekg(0);
		file.read((char*)buffer.GetData(), fileSize);

		file.close();

		return buffer;
	}
	std::string ReadTextFile(const std::string& path)
	{
		std::string data;
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		data.resize(fileSize);
		file.seekg(0);
		file.read(data.data(), fileSize);

		file.close();

		return data;
	}
}