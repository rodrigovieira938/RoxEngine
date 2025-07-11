#pragma once
#include <string>
#include <RoxEngine/utils/Utils.hpp>

namespace RoxEngine::FileSystem {
	class IFilesysten
	{
	public:
		~IFilesysten() = default;
		virtual bool IsResourcePath(const std::string& path) = 0;
		virtual bool Exists(const std::string& path) = 0;
		virtual std::string_view GetMimeType(const std::string& path) = 0;
		virtual std::string GetFileExtension(const std::string& path) = 0;
		virtual std::string GetFileName(const std::string& path, bool with_extension) = 0;
		virtual size_t GetFileSize(const std::string& path) = 0;
		virtual Buffer ReadFile(const std::string& path) = 0;
		virtual std::string ReadTextFile(const std::string& path) = 0;
	};

	bool IsResourcePath(const std::string& path);
	bool Exists(const std::string& path);
	std::string_view GetMimeType(const std::string& path);
	std::string GetFileExtension(const std::string& path);
	std::string GetFileName(const std::string& path, bool with_extension = true);
	size_t GetFileSize(const std::string& path);
	Buffer ReadFile(const std::string& path);
	std::string ReadTextFile(const std::string& path);
}