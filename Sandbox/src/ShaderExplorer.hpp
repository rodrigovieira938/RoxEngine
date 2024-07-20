#pragma once
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <array>

struct ShaderExplorer
{
	std::string inputPath = "res://shaders/basic.slang";
	std::string openFile;
	slang::IModule* module = nullptr;
	slang::ISession* session = nullptr;
	slang::IGlobalSession* globalSession = nullptr;
	std::vector<std::string> vertex_entry_points;
	std::vector<std::string> fragment_entry_points;

	const char* vertex_entrypoint_current = nullptr;
	const char* fragment_entrypoint_current = nullptr;

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	~ShaderExplorer()
	{
		if(module)
			module->release();
		if(session)
			session->release();
		if (globalSession)
			globalSession->release();
		module = nullptr;
		session = nullptr;
		globalSession = nullptr;
	}

private:
	void CompileModule(const std::string& src, const std::string& path)
	{
		if(!globalSession)
		{
			slang_createGlobalSession(SLANG_API_VERSION, &globalSession);
		}
		if (!session)
		{
			slang::SessionDesc session_desc;
			{
				static std::array<slang::TargetDesc, 1> targets;
				targets[0].format = SLANG_GLSL;
				targets[0].profile = globalSession->findProfile("glsl_450");
				static std::array<slang::PreprocessorMacroDesc, 0> preprocessor = {};


				session_desc.targets = targets.data();
				session_desc.targetCount = targets.size();
				session_desc.preprocessorMacros = preprocessor.data();
				session_desc.preprocessorMacroCount = preprocessor.size();
			}

			globalSession->createSession(session_desc, &session);
		}
		Slang::ComPtr<slang::IBlob> diagnostics;
		module = session->loadModuleFromSourceString(path.c_str(), path.c_str(), src.c_str(), diagnostics.writeRef());
		if (diagnostics)
		{
			std::cout << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
			module = nullptr;
			return;
		}

		for (int i = 0; i < module->getDefinedEntryPointCount(); i++)
		{
			slang::IEntryPoint* entry_point = nullptr;
			auto res = module->getDefinedEntryPoint(i, &entry_point);
			slang::ProgramLayout* layout = entry_point->getLayout();
			slang::EntryPointReflection* reflection = layout->getEntryPointByIndex(0);
			auto stage = reflection->getStage();

			if(reflection->getStage() == SLANG_STAGE_VERTEX)
			{
				vertex_entry_points.emplace_back(reflection->getName());
			}
			if (reflection->getStage() == SLANG_STAGE_FRAGMENT)
			{
				fragment_entry_points.emplace_back(reflection->getName());
			}
		}
		vertex_entry_points.emplace_back("Nothing##$"); // doller sign because it cant go in identifiers
		fragment_entry_points.emplace_back("Nothing##$");
		vertex_entrypoint_current = vertex_entry_points[vertex_entry_points.size()-1].c_str();
		fragment_entrypoint_current = fragment_entry_points[fragment_entry_points.size() - 1].c_str();
	}


public:

	void Render(std::string name = "ShaderExplorer")
	{
		if(ImGui::Begin((name + (openFile != "" ? " - " + openFile : "")).c_str(),nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Path to shader: ");
			ImGui::SameLine();
			ImGui::InputText("##inputPath", &inputPath);
			ImGui::SameLine();
			if(ImGui::Button("Open##inputPath_open"))
			{
				if(RoxEngine::FileSystem::Exists(inputPath))
				{
					openFile = inputPath;
					CompileModule(RoxEngine::FileSystem::ReadTextFile(openFile), openFile);
				}
			}
			if (ImGui::TreeNode("EntryPoints"))
			{
				if(module != nullptr)
				{
					for (int i = 0; i < module->getDefinedEntryPointCount(); i++)
					{
						slang::IEntryPoint* entry_point = nullptr;
						auto res = module->getDefinedEntryPoint(i, &entry_point);
						slang::ProgramLayout* layout = entry_point->getLayout();
						slang::EntryPointReflection* reflection = layout->getEntryPointByIndex(0);
						auto stage = reflection->getStage();

						if(ImGui::TreeNode(reflection->getName()))
						{
							for (int x = 0; x < reflection->getParameterCount(); x++) {
								slang::VariableLayoutReflection* param = reflection->getParameterByIndex(x);
								ImGui::Text("%s : %s", param->getName(), param->getType()->getName());
							}


							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::BeginCombo("##vertex_entrypoint_choose", vertex_entrypoint_current)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < vertex_entry_points.size(); n++)
				{
					bool is_selected = (vertex_entrypoint_current == vertex_entry_points[n].c_str()); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(vertex_entry_points[n].c_str(), is_selected))
						vertex_entrypoint_current = vertex_entry_points[n].c_str();
						if (is_selected)
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
				ImGui::EndCombo();
			}
			if (ImGui::BeginCombo("##fragment_entrypoint_choose", fragment_entrypoint_current)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < fragment_entry_points.size(); n++)
				{
					bool is_selected = (fragment_entrypoint_current == fragment_entry_points[n].c_str()); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(fragment_entry_points[n].c_str(), is_selected))
						fragment_entrypoint_current = fragment_entry_points[n].c_str();
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
				ImGui::EndCombo();
			}

			if(ImGui::Button("Compile") && module != nullptr)
			{
				 {
					slang::IEntryPoint* vertex_entrypoint  = nullptr;
					slang::IEntryPoint* fragment_entrypoint = nullptr;
				 	module->findEntryPointByName(vertex_entrypoint_current, &vertex_entrypoint);
				 	module->findEntryPointByName(fragment_entrypoint_current, &fragment_entrypoint);

					slang::IComponentType* components[] = { module, vertex_entrypoint, fragment_entrypoint };

					slang::IComponentType* program;
					session->createCompositeComponentType(components, 3, &program);

					slang::IComponentType* linkedProgram;
					slang::IBlob* diagnosticBlob = nullptr;
					program->link(&linkedProgram, &diagnosticBlob);

					if(diagnosticBlob)
					{
						std::cout << static_cast<const char*>(diagnosticBlob->getBufferPointer()) << "\n";
					}
					else
					{
						slang::IBlob* diagnostics = nullptr;
						if (std::strcmp(vertex_entrypoint_current, "Nothing##$") != 0)
						{
							slang::IBlob* code;
							linkedProgram->getEntryPointCode(0, 0, &code, &diagnostics);

							vertexShaderCode = std::string(static_cast<const char*>(code->getBufferPointer()));

							code->release();
							if(diagnostics)
								diagnostics->release();
						}
						if (std::strcmp(fragment_entrypoint_current, "Nothing##$") != 0)
						{
							diagnostics = nullptr;
							slang::IBlob* code;
							linkedProgram->getEntryPointCode(1, 0, &code, &diagnostics);

							fragmentShaderCode = std::string(static_cast<const char*>(code->getBufferPointer()));

							code->release();
							if (diagnostics)
								diagnostics->release();
						}

						linkedProgram->release();
					}

					program->release();
					if(diagnosticBlob)
				 		diagnosticBlob->release();
					program->release();
					if(vertex_entrypoint)
						vertex_entrypoint->release();
					if (fragment_entrypoint)
						fragment_entrypoint->release();
				}
				 if (!vertexShaderCode.empty())
					 std::cout << vertexShaderCode << "\n\n\n\n";
				 if (!fragmentShaderCode.empty())
					 std::cout << fragmentShaderCode << "\n";
			}
		}
		ImGui::End();
	}

};