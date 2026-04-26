#pragma once
#include <unordered_map>
#include <string>

static std::unordered_map<std::string, std::string_view> mimeTypeDB = {
	{".txt", "text/plain"},
	{".gltf", "model/gltf+json"},
	{".obj", "model/obj"},
	{".fbx", "model/fbx"}
};