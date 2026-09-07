#include <RoxEngine/filesystem/Filesystem.hpp>
#include <slang.h>
#include <string>
namespace RoxEngine::SlangLayer {
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
}