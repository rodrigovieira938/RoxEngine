#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/filesystem/Filesystem.hpp"
#include <RoxEngine/slang/slang.hpp>

namespace RoxEngine {    
    Slang::ComPtr<slang::IGlobalSession> sGlobalSession = nullptr;
    Slang::ComPtr<slang::ISession> sSession = nullptr;

    class stringblob final : public slang::IBlob
    {
    public:
        stringblob(std::string&& content) : mContent(std::move(content)) {}
        virtual ~stringblob() = default;
        SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override { return SLANG_FAIL; }
        SLANG_NO_THROW uint32_t addRef() override { return mRefCount++; }
        SLANG_NO_THROW uint32_t release() override {
            mRefCount--;
            if (mRefCount == 0)
            {
                delete this;
            }
            return 1;
        }
        SLANG_NO_THROW const void* getBufferPointer()  override {return mContent.data();}
        SLANG_NO_THROW size_t getBufferSize() override {return mContent.size();}

    private:
        std::string mContent;
        uint32_t mRefCount = 1;
    };

    struct filesystem final : public ISlangFileSystem
    {
        virtual ~filesystem() = default;
        SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override { return SLANG_FAIL; }
        SLANG_NO_THROW void* castAs(const SlangUUID& guid) override {
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
        SLANG_NO_THROW uint32_t release() override {
            mRefCount--;
            if (mRefCount == 0)
            {
                delete this;
            }
            return 1;
        }
        SLANG_NO_THROW SlangResult loadFile(const char* path, ISlangBlob** outBlob) override {
            if (!FileSystem::Exists(path))
                return SLANG_E_NOT_FOUND;
            *outBlob = new stringblob(FileSystem::ReadTextFile(path));
            return SLANG_OK;
        }
    private:
        uint32_t mRefCount = 1;
    };

	void SlangLayer::Init()
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

                session_desc.fileSystem = new filesystem();

                static constexpr const char* search_paths[] = {
                    "" //allow for absolute include path
                };

                session_desc.searchPaths = search_paths;
                session_desc.searchPathCount = 1;
            }

            sGlobalSession->createSession(session_desc, sSession.writeRef());
        }
        if(!sGlobalSession)
        {
            //TODO: instead of printing return this in an error
            log::fatal("Failed to create slang's Global Session");
        }
	}
    Slang::ComPtr<slang::IModule> SlangLayer::CompileModule(const std::string& filepath) {
        Slang::ComPtr<slang::IBlob> diagnostics;
        std::string module_name = FileSystem::GetFileName(filepath,false);
        std::string src = FileSystem::ReadTextFile(filepath);
        Slang::ComPtr<slang::IModule> module;
        module = sSession->loadModuleFromSourceString(module_name.c_str(), filepath.c_str(), src.c_str(), diagnostics.writeRef());
        if (diagnostics)
        {
            log::error("Shader loading failed: {}", (char*)diagnostics->getBufferPointer());
            return nullptr;
        }
        return module;
    }
    void SlangLayer::Shutdown()
    {
        sSession->release();
        sGlobalSession->release();
    }
}
