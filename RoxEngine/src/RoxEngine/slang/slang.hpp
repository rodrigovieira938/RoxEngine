#pragma once
#include <string>
#include "include/slang-com-ptr.h"
#include "include/slang.h"

namespace RoxEngine {
    class SlangLayer
    {
    public:
        class stringblob final : public slang::IBlob
        {
        public:
            stringblob(std::string&& content);
            virtual ~stringblob() = default;
            SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override;
            SLANG_NO_THROW uint32_t addRef() override;
            SLANG_NO_THROW uint32_t release() override;
            SLANG_NO_THROW const void* getBufferPointer()  override;
            SLANG_NO_THROW size_t getBufferSize() override;

        private:
            std::string mContent;
            uint32_t mRefCount = 1;
        };

        struct filesystem final : public ISlangFileSystem
        {
            virtual ~filesystem() = default;
            SLANG_NO_THROW SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override;
            SLANG_NO_THROW void* castAs(const SlangUUID& guid) override;
            SLANG_NO_THROW uint32_t addRef() override;
            SLANG_NO_THROW uint32_t release() override;
            SLANG_NO_THROW SlangResult loadFile(const char* path, ISlangBlob** outBlob) override;
        private:
            uint32_t mRefCount = 1;
        };

        static void Init();
        static void Shutdown();
        static Slang::ComPtr<slang::IGlobalSession> sGlobalSession;
        static Slang::ComPtr<slang::ISession> sSession;
    };
};