#include "GLShader.hpp"
#include <glad/gl.h>
#include <iostream>
#include <RoxEngine/filesystem/Filesystem.hpp>

#include "slang-com-ptr.h"

namespace RoxEngine::GL {
    GLuint CreateProgram(const char* vertexSource, const char* fragmentSource) {
        std::cout << vertexSource << "\n\n" << fragmentSource << "\n";

        const GLuint id = glCreateProgram();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
    	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);


    	glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return id;
    }

    Slang::ComPtr<slang::IGlobalSession> sGlobalSession = nullptr;
    Slang::ComPtr<slang::ISession> sSession = nullptr;

    class StringBlob final: public slang::IBlob
    {
    public:
        StringBlob(std::string&& content) : mContent(std::move(content)) {}
        virtual ~StringBlob() = default;
        SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override { return SLANG_FAIL; }
        SLANG_NO_THROW uint32_t addRef() override
        {
	        return mRefCount++;
        }
        SLANG_NO_THROW uint32_t release() override
        {
            mRefCount--;
            if (mRefCount == 0)
            {
                delete this;
            }
            return 1;
        }
        SLANG_NO_THROW const void* getBufferPointer()  override { return mContent.data(); }
        SLANG_NO_THROW size_t getBufferSize() override { return mContent.size(); }

    private:
        std::string mContent;
        uint32_t mRefCount = 1;
    };

    struct slangfs final : public ISlangFileSystem
    {
    public:
        virtual ~slangfs() = default;
        SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override { return SLANG_FAIL; }
        SLANG_NO_THROW void* castAs(const SlangUUID& guid) override
	    {
            if (guid == ISlangUnknown::getTypeGuid() ||
                guid == ISlangFileSystem::getTypeGuid())
            {
                return static_cast<ISlangFileSystem*>(this);
            }
            if (guid == ISlangCastable::getTypeGuid())
            {
                return static_cast<ISlangCastable*>(this);
            }
            return nullptr;
	    }
        SLANG_NO_THROW uint32_t addRef() override { return mRefCount++; }
        SLANG_NO_THROW uint32_t release() override
        {
            mRefCount--;
            if (mRefCount == 0)
            {
                delete this;
            }
            return 1;
        }
        SLANG_NO_THROW SlangResult loadFile(const char* path, ISlangBlob** outBlob) override
	    {
            if(!FileSystem::Exists(path))
				return SLANG_E_NOT_FOUND;
            *outBlob = new StringBlob(FileSystem::ReadTextFile(path));
            return SLANG_OK;
	    }

        
    private:
        uint32_t mRefCount = 1;
    };

    void initSlang()
    {
        if (!sGlobalSession)
        {
            //TODO: deal with SlangResults

            slang::createGlobalSession(sGlobalSession.writeRef());

            slang::SessionDesc session_desc;
            {
                static slang::TargetDesc targets[] = {
                    {
                            sizeof(slang::TargetDesc),
                            SLANG_GLSL,
                            SLANG_PROFILE_UNKNOWN,
                            kDefaultTargetFlags,
                            SLANG_FLOATING_POINT_MODE_DEFAULT,
                            SLANG_LINE_DIRECTIVE_MODE_DEFAULT,
                            false,
                            nullptr,
                            0,
                    }
                };
                targets[0].profile = sGlobalSession->findProfile("glsl_450");

                session_desc.targets = targets;
                session_desc.targetCount = sizeof(slang::TargetDesc) / sizeof(targets);

                session_desc.fileSystem = new slangfs();

                static const char* const search_paths[] = {
                    "" //allow for absolute include path
                };

                session_desc.searchPaths = search_paths;
                session_desc.searchPathCount = 1;
            }

            sGlobalSession->createSession(session_desc, sSession.writeRef());
        }
    }

    GLuint CompileModule(const Slang::ComPtr<slang::IModule>& module, const Shader::EntryPointInfo& entry_point)
    {
        Slang::ComPtr<slang::IEntryPoint> vertex_entrypoint = nullptr;
        Slang::ComPtr<slang::IEntryPoint> fragment_entrypoint = nullptr;
        //TODO: make the roxengine.slang module with default entry points
        if (module->findEntryPointByName(entry_point.vertex_main, vertex_entrypoint.writeRef()))
            module->findEntryPointByName("default_vertex_main", vertex_entrypoint.writeRef());
        if (module->findEntryPointByName(entry_point.fragment_main, fragment_entrypoint.writeRef()))
            module->findEntryPointByName("default_fragment_main", fragment_entrypoint.writeRef());

        slang::IComponentType* components[] = { module, vertex_entrypoint, fragment_entrypoint };

        Slang::ComPtr<slang::IComponentType> program;
        sSession->createCompositeComponentType(components, 3, program.writeRef());
    	Slang::ComPtr<slang::IBlob> diagnosticBlob = nullptr;
        Slang::ComPtr<slang::IComponentType> linkedProgram = nullptr;
        program->link(linkedProgram.writeRef(), diagnosticBlob.writeRef());

        if (diagnosticBlob)
        {
            std::cout << static_cast<const char*>(diagnosticBlob->getBufferPointer()) << "\n";
            return 0;
        }

        Slang::ComPtr<slang::IBlob> vertex_diagnostic, fragment_diagnostic;
        Slang::ComPtr<slang::IBlob> vertex_code, fragment_code;


        linkedProgram->getEntryPointCode(0, 0, vertex_code.writeRef(), vertex_diagnostic.writeRef());
        linkedProgram->getEntryPointCode(1, 0, fragment_code.writeRef(), fragment_diagnostic.writeRef());

        if (vertex_diagnostic || fragment_diagnostic)
        {
            if (vertex_diagnostic)
                std::cout << "VERTEX SHADER ERROR: " << static_cast<const char*>(vertex_diagnostic->getBufferPointer()) << "\n";
            if (fragment_diagnostic)
                std::cout << "FRAGMENT SHADER ERROR: " << static_cast<const char*>(fragment_diagnostic->getBufferPointer()) << "\n";
            return 0;
        }

        return CreateProgram(
            static_cast<const char*>(vertex_code->getBufferPointer()),
            static_cast<const char*>(fragment_code->getBufferPointer()));
    }

    Shader::Shader(const std::string& src, const std::string& module_name, const EntryPointInfo& entry_point)
    {
        initSlang();
        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IModule> module;

    	module = sSession->loadModuleFromSourceString(module_name.c_str(), "", src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            std::cout << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
            return;
        }
        mID = CompileModule(module, entry_point);
    }

    Shader::Shader(const std::string& path, const EntryPointInfo& entry_point)
    {
        initSlang();
        std::string src = FileSystem::ReadTextFile(path);

        Slang::ComPtr<slang::IBlob> diagnostics;
        Slang::ComPtr<slang::IModule> module;
        std::string module_name = FileSystem::GetFileName(path,false);
        module = sSession->loadModuleFromSourceString(module_name.c_str(), path.c_str(), src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            std::cout << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
            return;
        }
        mID = CompileModule(module, entry_point);
    }

    Shader::~Shader() {
        glDeleteProgram(mID);
    }
}
