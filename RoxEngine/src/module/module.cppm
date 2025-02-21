module;
#include <RoxEngine/roxengine.hpp>
export module roxengine;
namespace RoxEngine {
    /*assetmanager/AssetManager.hpp*/
    namespace AssetManager {
        export using RoxEngine::AssetManager::AssetDecoder;
        export using RoxEngine::AssetManager::TextAssetDecoder;
        export using RoxEngine::AssetManager::Load;
    }
    /*assetmanager/AssetManager.hpp*/
    /*core/Engine.hpp */
    export using RoxEngine::Game;
    export using RoxEngine::Engine;
    /*core/Engine.hpp */
    /*core/Errors.hpp */
    export using RoxEngine::IError;
    export using RoxEngine::RuntimeError;
    export using RoxEngine::ResultValueAcessedError;
	export using RoxEngine::ResultErrorAcessedError;
	export using RoxEngine::EntityNotAlive;
	export using RoxEngine::GetComponentNo;
	export using RoxEngine::AddComponentTwice;
	export using RoxEngine::RemoveComponentNo;
    /*core/Errors.hpp */
    /*core/Logger.hpp */
    namespace log {
        export using RoxEngine::log::Logger;
        export using RoxEngine::log::trace;
        export using RoxEngine::log::info;
        export using RoxEngine::log::warn;
        export using RoxEngine::log::error;
        export using RoxEngine::log::fatal;
    }
    /*core/Logger.hpp */
    /*core/Panic.hpp */
    export using RoxEngine::PanicFunc;
    export using RoxEngine::SetPanicHandler;
    export using RoxEngine::SetGlobalPanicHandler;
    export using RoxEngine::Panic;
    /*core/Panic.hpp */
    /*core/Result.hpp */
    export using RoxEngine::ResultDeref;
    export using RoxEngine::Result;
    /*core/Result.hpp */
    /*core/Window.hpp */
    export using RoxEngine::Window;
    /*core/Window.hpp */
    /*filesystem/Filesystem.hpp */
    namespace FileSystem {
        export using RoxEngine::FileSystem::IsResourcePath;
        export using RoxEngine::FileSystem::Exists;
        export using RoxEngine::FileSystem::GetCanonicalPath;
        export using RoxEngine::FileSystem::GetMimeType;
        export using RoxEngine::FileSystem::GetFileExtension;
        export using RoxEngine::FileSystem::GetFileName;
        export using RoxEngine::FileSystem::ReadFile;
        export using RoxEngine::FileSystem::ReadTextFile;
    }
    /*filesystem/Filesystem.hpp */
    /*imgui/Imgui.hpp */
    //export using RoxEngine::ImGuiLayer; // A game shouln't have acess to ImGuiLayer
    /*core/Imgui.hpp */
    /*input/input.hpp */
    export using RoxEngine::KeyCode;
    namespace Key {
        export using namespace RoxEngine::Key;
    }
    export using RoxEngine::KeyState;
    export using RoxEngine::Input;
    /*input/input.hpp */
    //platforms/*
    // Should be in a different module - roxengine.platforms
    //platforms/*
    //profiler/*
    // A module cant use ifdefs
    //profiler/*
    /*renderer/Buffer.hpp*/
    export using RoxEngine::ShaderDataType;
    export using RoxEngine::BufferElement;
    export using RoxEngine::BufferLayout;
    export using RoxEngine::VertexBuffer;
    export using RoxEngine::IndexBuffer;
    export using RoxEngine::FramebufferColorTexFormat;
    export using RoxEngine::FramebufferDepthTexFormat;
    export using RoxEngine::Framebuffer;
    /*renderer/Buffer.hpp*/
    /*renderer/GraphicsContext.hpp*/
    export using RoxEngine::RendererApi;
    export using RoxEngine::ClearScreenMask;
    namespace ClearScreen {
        export using RoxEngine::ClearScreen::ClearScreen;
    }
    export using RoxEngine::GraphicsContext;
    /*renderer/GraphicsContext.hpp*/
    /*renderer/Material.hpp*/    
    export using RoxEngine::Material;
    /*renderer/Material.hpp*/
    /*renderer/Renderer.hpp*/
    export using RoxEngine::MeshTopology;
    export using RoxEngine::SubMeshInfo;
    export using RoxEngine::Mesh;
    /*renderer/Renderer.hpp*/
    /*renderer/Shader.hpp*/
    export using RoxEngine::Shader;
    /*renderer/Shader.hpp*/
    /*renderer/UniformBuffer.hpp*/
    export using RoxEngine::UniformBuffer;
    /*renderer/UniformBuffer.hpp*/
    /*renderer/VertexArray.hpp*/
    export using RoxEngine::VertexArray;
    /*renderer/Scene.hpp*/
    export using RoxEngine::Scene;
    export using RoxEngine::Entity;
    /*renderer/Scene.hpp*/
    /*renderer/Utils.hpp*/
    export using RoxEngine::Scope;
    export using RoxEngine::Ref;
    export using RoxEngine::CreateScope;
    export using RoxEngine::CreateRef;
    export using RoxEngine::Buffer;
    export using RoxEngine::ScopedBuffer;
    /*renderer/Utils.hpp*/
}