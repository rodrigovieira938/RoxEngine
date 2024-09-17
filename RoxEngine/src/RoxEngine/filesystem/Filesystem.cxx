module;
#include <string>
export module roxengine:filesystem;
import :utils;

namespace RoxEngine::FileSystem {
	export class IFilesysten
	{
	public:
		~IFilesysten() = default;
		virtual bool IsResourcePath(const std::string& path) = 0;
		virtual bool Exists(const std::string& path) = 0;

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
		bool Exists(const std::string& path) override;
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
		bool Exists(const std::string& path) { return impl.Exists(path); }
		std::string GetFileName(const std::string& path, bool with_extension) { return impl.GetFileName(path, with_extension); }
		size_t GetFileSize(const std::string& path) { return impl.GetFileSize(path); }
		Buffer ReadFile(const std::string& path) { return impl.ReadFile(path); }
		std::string ReadTextFile(const std::string& path) { return impl.ReadTextFile(path); }

	private:
		FileSystemImpl impl;
	};

	inline static FileSystem sFileSystem;

	export bool IsResourcePath(const std::string& path) { return sFileSystem.IsResourcePath(path); }
	export bool Exists(const std::string& path) { return sFileSystem.Exists(path); }
	export std::string GetFileName(const std::string& path, bool with_extension = true) { return sFileSystem.GetFileName(path, with_extension);  }
	export size_t GetFileSize(const std::string& path) { return sFileSystem.GetFileSize(path); }
	export Buffer ReadFile(const std::string& path) { return sFileSystem.ReadFile(path); }
	export std::string ReadTextFile(const std::string& path) { return sFileSystem.ReadTextFile(path); }
}