#pragma once
#include <string>
#include <RoxEngine/filesystem/Filesystem.hpp>

namespace RoxEngine::AssetManager {
	template<typename T>
	concept AssetDecoder = requires(T a, const std::string_view& view, const std::string_view & view2) {
		{ a.IsSameMimeType(view) } -> std::same_as<bool>;
		{ a.Decode(view) } -> std::same_as<void>;
	};

	struct TextAssetDecoder
	{
		std::string content;
		static constexpr bool IsSameMimeType(const std::string_view& view)
		{
			return view == "text/plain";
		}
		void Decode(const std::string_view& path)
		{
			content = FileSystem::ReadTextFile(path.data());
		}
	};

	template<typename T> requires AssetDecoder<T>
	bool Load(const std::string& filepath, T& decoder)
	{
		bool same = decoder.IsSameMimeType(FileSystem::GetMimeType(filepath));
		if(same)
		{
			decoder.Decode(filepath);
		}
		return same;
	}
};