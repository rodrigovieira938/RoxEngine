export module roxengine:renderer;

export import :renderer_buffers;
export import :renderer_graphicscontext;
export import :renderer_material;
export import :renderer_shader;
export import :renderer_uniformbuffer;
export import :renderer_vertexarray;
import "glm/fwd.hpp";
import "glm/vec2.hpp";
import "glm/vec4.hpp";
import "glm/mat4x4.hpp";

export namespace RoxEngine
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

	//Basic renderer implementation
	export class IRenderer
	{
		//Returns camera's id for this frame
		virtual size_t SetCamera(glm::mat4 viewProj, glm::mat4 camPos) = 0;
		//CameraID=0 for last set camera
		virtual void DrawMesh(Mesh mesh, size_t cameraID = 0) = 0;
		virtual void Render() const = 0;
		virtual void Clear() = 0;
	};
	export class DefaultRenderer : public IRenderer
	{
	private:
		struct Camera
		{
			glm::mat4 viewProj, camPos;
		};
		struct Batch
		{
			size_t camIndex;
			Ref<VertexArray> va;
		};
		std::vector<Camera> cams;
		std::vector<Batch> batches;
	public:
		size_t SetCamera(glm::mat4 viewProj, glm::mat4 camPos) override
		{
			cams.push_back({ viewProj, camPos });
			return cams.size() - 1;
		}
		void DrawMesh(Mesh mesh, size_t cameraID = 0) override
		{
			Batch b;
			if (cameraID == 0)
				b.camIndex = cams.size() - 1;
			else
				b.camIndex = cameraID;
			b.va = VertexArray::Create();
			b.va->SetIndexBuffer(IndexBuffer::Create(mesh.indices.data(), mesh.indices.size()));
			#define SET_VA(attr, sizef) if(mesh.attr.size() > 0) { auto vb = VertexBuffer::Create(mesh.attr.data(), mesh.attr.size() * (sizef));vb->SetLayout({ {#attr, ShaderDataType::Float3} }); b.va->AddVertexBuffer(vb);}
			SET_VA(position, sizeof(glm::vec3));
			SET_VA(uv, sizeof(glm::vec2));
			SET_VA(uv2, sizeof(glm::vec2));
			SET_VA(uv3, sizeof(glm::vec2));
			SET_VA(uv4, sizeof(glm::vec2));
			SET_VA(uv5, sizeof(glm::vec2));
			SET_VA(uv6, sizeof(glm::vec2));
			SET_VA(uv7, sizeof(glm::vec2));
			SET_VA(uv8, sizeof(glm::vec2));
			SET_VA(normal, sizeof(glm::vec3));
			SET_VA(color, sizeof(glm::vec4));
			batches.emplace_back(b);
		}
		void Render() const override
		{
			for(auto& batch : batches)
			{
				GraphicsContext::Draw(batch.va, batch.va->GetIndexBuffer()->GetCount());
			}
		}
		void Clear() override
		{
			cams.clear();
			batches.clear();
		}
		virtual ~DefaultRenderer() = default;
	};
}
