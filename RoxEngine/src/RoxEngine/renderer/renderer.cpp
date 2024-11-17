#include <imgui/imgui.h>
#include <glad/gl.h>
#include <RoxEngine/renderer/Renderer.hpp>
#include <RoxEngine/renderer/GraphicsContext.hpp>

#include <RoxEngine/platforms/GL/GLVertexArray.hpp>
#include <RoxEngine/platforms/GL/GLBuffer.hpp>

namespace RoxEngine {
	void DefaultRenderer::Render() const {
		if(debugNormalRendering)
		{
			for (auto& batch : batches)
			{
				GraphicsContext::Draw(batch.va, batch.va->GetIndexBuffer()->GetCount());
			}
		}
		if (debugWireMesh && debugMaterial != nullptr)
		{
			debugMaterial->GetUbo("Uniforms")->Set("color", (void*)&debugWireMeshColor.r, sizeof(debugWireMeshColor));
			glLineWidth(debugWireMeshLineSize);
			GraphicsContext::UseMaterial(debugMaterial);
			for (auto& batch : batches)
			{
				auto glva = std::static_pointer_cast<GL::VertexArray>(batch.va);
				glBindVertexArray(glva->mID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::static_pointer_cast<GL::IndexBuffer>(glva->mIB)->mID);
				glDrawElements(GL_LINE_STRIP, static_cast<int>(glva->mIB->GetCount()), GL_UNSIGNED_INT, nullptr);
			}
		}
	}
	void DefaultRenderer::DebugMenu() {
		ImGui::Begin("DefaultRenderer - Debug Menu");
		ImGui::Checkbox("Normal rendering", &debugNormalRendering);
		ImGui::Checkbox("Wiremesh rendering", &debugWireMesh);
		ImGui::ColorPicker3("Wiremesh color", &debugWireMeshColor.r);
		ImGui::DragFloat("Wiremesh Line Size", &debugWireMeshLineSize);
		ImGui::End();
	}
}