module;
#include <filesystem>
#include <fstream>
#include <iostream>
#include "MimeTypeDB.hpp"
module roxengine;
namespace fs = std::filesystem;
namespace RoxEngine::FileSystem {
	bool FileFilesystem::IsResourcePath(const std::string& path)
	{
		return path.rfind("res://", 0) == 0;
	}
	bool FileFilesystem::Exists(const std::string& path)
	{
		if (IsResourcePath(path))
		{
			return fs::exists(std::string(mResourcesFolder + std::string(path.begin() + 5, path.end())));
		}
		return fs::exists(fs::path(path));
	}

	std::string_view FileFilesystem::GetMimeType(const std::string& path)
	{
		if(Exists(path))
		{
			auto fspath = fs::path(path);
			auto extension = fspath.extension().generic_string();
			auto it = mimeTypeDB.find(extension);
			if (it != mimeTypeDB.end())
				return it->second;
			return "unknown";
		}
		//TODO: return error
		return "";
	}

	std::string FileFilesystem::GetFileExtension(const std::string& path)
	{
		fs::path fspath = fs::path(path);
		if (IsResourcePath(path))
		{
			fspath = std::string(mResourcesFolder + std::string(path.begin() + 5, path.end()));
		}
		return fspath.extension().generic_string();
	}

	std::string FileFilesystem::GetFileName(const std::string& path, bool with_extension)
	{
		fs::path fspath = fs::path(path);
		if(IsResourcePath(path))
		{
			fspath = std::string(mResourcesFolder + std::string(path.begin() + 5, path.end()));
		}

		if (with_extension)
			return fspath.filename().generic_string();
		return fspath.stem().generic_string();
	}

	size_t FileFilesystem::GetFileSize(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		return (size_t)file.tellg();
	}
	Buffer FileFilesystem::ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if(IsResourcePath(path))
		{
			std::string true_path = fs::canonical(std::string(mResourcesFolder + std::string(path.begin() + 5, path.end()))).generic_string();
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
	std::string FileFilesystem::ReadTextFile(const std::string& path)
	{
		std::string data;
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (IsResourcePath(path))
		{
			std::string true_path = fs::canonical(std::string(mResourcesFolder + std::string(path.begin() + 5, path.end()))).generic_string();
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