#pragma once
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <RoxEngine/renderer/VertexArray.hpp>
#include <RoxEngine/renderer/Buffer.hpp>
#include <RoxEngine/renderer/Material.hpp>

namespace RoxEngine
{
	enum class MeshTopology
	{
		TRIANGLES,
		QUADS,
		LINES,
		LINESTRIP,
		POINTS
	};
	struct SubMeshInfo
	{
		size_t firstVertex = 0;
		size_t indexCount = 0;
	};
	struct Mesh
	{
		std::vector<glm::vec3> position;
		std::vector<glm::vec2> uv, uv2, uv3, uv4, uv5, uv6, uv7, uv8;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec3> color;
		MeshTopology topology = MeshTopology::TRIANGLES; //if there are submeshes this field is ignored
		std::vector<uint32_t> indices;
		std::vector<SubMeshInfo> subMeshInfo; //if size is zero all the vertices belong to 1 mesh
	};
}
