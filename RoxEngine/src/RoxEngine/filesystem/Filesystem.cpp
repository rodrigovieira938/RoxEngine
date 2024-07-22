#include "Filesystem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
namespace RoxEngine::FileSystem {
	std::string resourcesFolder;
	bool IsResourcePath(const std::string& path)
	{
		return path.rfind("res://", 0) == 0;
	}
	bool Exists(const std::string& path)
	{
		if (IsResourcePath(path))
		{
			return fs::exists(std::string(resourcesFolder + std::string(path.begin() + 5, path.end())));
		}
		return fs::exists(fs::path(path));
	}

	std::string GetFileName(const std::string& path, bool with_extension)
	{
		fs::path fspath = fs::path(path);
		if(IsResourcePath(path))
		{
			fspath = std::string(resourcesFolder + std::string(path.begin() + 5, path.end()));
		}

		if (with_extension)
			return fspath.filename().generic_string();
		return fspath.stem().generic_string();
	}

	size_t GetFileSize(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		return (size_t)file.tellg();
	}
	Buffer ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if(IsResourcePath(path))
		{
			std::string true_path = fs::canonical(std::string(resourcesFolder + std::string(path.begin() + 5, path.end()))).generic_string();
			file = std::ifstream(true_path, std::ios::ate | std::ios::binary);
		}

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		Buffer buffer(fileSize);
		file.seekg(0);
		file.read(static_cast<char*>(buffer.GetData()), static_cast<std::streamsize>(fileSize));

		file.close();

		return buffer;
	}
	std::string ReadTextFile(const std::string& path)
	{
		std::string data;
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (IsResourcePath(path))
		{
			std::string true_path = fs::canonical(std::string(resourcesFolder + std::string(path.begin() + 5, path.end()))).generic_string();
			file = std::ifstream(true_path, std::ios::ate | std::ios::binary);
		}
		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		data.resize(fileSize);
		file.seekg(0);
		file.read(data.data(), static_cast<std::streamsize>(fileSize));

		file.close();

		return data;
	}
}