#pragma once
#include <string>
#include <RoxEngine/utils/Utils.hpp>

namespace RoxEngine::FileSystem {
	class IFilesysten
	{
	public:
		~IFilesysten() = default;
		virtual bool IsResourcePath(const std::string& path) = 0;
		virtual std::string GetCanonicalPath(const std::string& path) = 0;
		virtual bool Exists(const std::string& path) = 0;
		virtual std::string_view GetMimeType(const std::string& path) = 0;
		virtual std::string GetFileExtension(const std::string& path) = 0;
		virtual std::string GetFileName(const std::string& path, bool with_extension) = 0;
		virtual size_t GetFileSize(const std::string& path) = 0;
		virtual Buffer ReadFile(const std::string& path) = 0;
		virtual std::string ReadTextFile(const std::string& path) = 0;
	};
	class FileFilesystem final : public IFilesysten
	{
		public:
		FileFilesystem(std::string&& folder) : mResourcesFolder(std::move(folder)) {}
		std::string mResourcesFolder;
		bool IsResourcePath(const std::string& path) override;
		std::string GetCanonicalPath(const std::string& path) override;
		bool Exists(const std::string& path) override;
		std::string_view GetMimeType(const std::string& path) override;
		std::string GetFileExtension(const std::string& path) override;
		std::string GetFileName(const std::string& path, bool with_extension) override;
		size_t GetFileSize(const std::string& path) override;
		Buffer ReadFile(const std::string& path) override;
		std::string ReadTextFile(const std::string& path) override;
	};
	using FileSystemImpl = FileFilesystem;
	using FileSystem = class FilesystemProxy
	{
	public:
		FilesystemProxy()
			: impl("./resources")
		{}

		bool IsResourcePath(const std::string& path) { return impl.IsResourcePath(path); }
		std::string GetCanonicalPath(const std::string& path) { return impl.GetCanonicalPath(path); }
		bool Exists(const std::string& path) { return impl.Exists(path); }
		std::string_view GetMimeType(const std::string& path) { return impl.GetMimeType(path); }
		std::string GetFileExtension(const std::string& path) { return impl.GetFileExtension(path); }
		std::string GetFileName(const std::string& path, bool with_extension) { return impl.GetFileName(path, with_extension); }
		size_t GetFileSize(const std::string& path) { return impl.GetFileSize(path); }
		Buffer ReadFile(const std::string& path) { return impl.ReadFile(path); }
		std::string ReadTextFile(const std::string& path) { return impl.ReadTextFile(path); }

	private:
		FileSystemImpl impl;
	};

	extern FileSystem sFileSystem;

	inline bool IsResourcePath(const std::string& path) { return sFileSystem.IsResourcePath(path); }
	inline bool Exists(const std::string& path) { return sFileSystem.Exists(path); }
	inline std::string GetCanonicalPath(const std::string& path) { return sFileSystem.GetCanonicalPath(path); }
	inline std::string_view GetMimeType(const std::string& path) { return sFileSystem.GetMimeType(path); }
	inline std::string GetFileExtension(const std::string& path) { return sFileSystem.GetFileExtension(path); }
	inline std::string GetFileName(const std::string& path, bool with_extension = true) { return sFileSystem.GetFileName(path, with_extension);  }
	inline size_t GetFileSize(const std::string& path) { return sFileSystem.GetFileSize(path); }
	inline Buffer ReadFile(const std::string& path) { return sFileSystem.ReadFile(path); }
	inline std::string ReadTextFile(const std::string& path) { return sFileSystem.ReadTextFile(path); }
}