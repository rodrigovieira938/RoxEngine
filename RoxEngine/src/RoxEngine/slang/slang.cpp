#include "slang.hpp"

#include <iostream>

#include "RoxEngine/filesystem/Filesystem.hpp"

namespace RoxEngine {

    Slang::ComPtr<slang::IGlobalSession> SlangLayer::sGlobalSession = nullptr;
    Slang::ComPtr<slang::ISession> SlangLayer::sSession = nullptr;

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
            std::cout << "OHHH NOO!";
        }
	}
    void SlangLayer::Shutdown()
    {
        sSession->release();
        sGlobalSession->release();
    }
    SLANG_NO_THROW SlangResult SlangLayer::filesystem::queryInterface(const SlangUUID& uuid, void** outObject) { return SLANG_FAIL; }
    SLANG_NO_THROW void* SlangLayer::filesystem::castAs(const SlangUUID& guid)
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
    SLANG_NO_THROW uint32_t SlangLayer::filesystem::addRef() { return mRefCount++; }
    SLANG_NO_THROW uint32_t SlangLayer::filesystem::release()
    {
        mRefCount--;
        if (mRefCount == 0)
        {
            delete this;
        }
        return 1;
    }
    SLANG_NO_THROW SlangResult SlangLayer::filesystem::loadFile(const char* path, ISlangBlob** outBlob)
    {
        if (!FileSystem::Exists(path))
            return SLANG_E_NOT_FOUND;
        *outBlob = new stringblob(FileSystem::ReadTextFile(path));
        return SLANG_OK;
    }
    SlangLayer::stringblob::stringblob(std::string&& content) : mContent(std::move(content)) {}
    SLANG_NO_THROW SlangResult SlangLayer::stringblob::queryInterface(const SlangUUID& uuid, void** outObject) { return SLANG_FAIL; }
    SLANG_NO_THROW uint32_t SlangLayer::stringblob::addRef()
    {
        return mRefCount++;
    }
    SLANG_NO_THROW uint32_t SlangLayer::stringblob::release()
    {
        mRefCount--;
        if (mRefCount == 0)
        {
            delete this;
        }
        return 1;
    }
    SLANG_NO_THROW const void* SlangLayer::stringblob::getBufferPointer()  { return mContent.data(); }
    SLANG_NO_THROW size_t SlangLayer::stringblob::getBufferSize() { return mContent.size(); }
}
