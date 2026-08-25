#pragma once
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <RoxEngine/utils/Utils.hpp>
#include "slang-com-ptr.h"
#include "slang.h"
#include <RoxEngine/renderer/ShaderReflection.hpp>
#include <RoxEngine/filesystem/Filesystem.hpp>

namespace RoxEngine {
    struct ModuleReflection {
        struct Field {
            std::string name;
            size_t offset;
            const ShaderReflection::Type* type = nullptr;
        };
        struct UniformBuffer {
            std::string name;
            size_t size;
            std::vector<Field> fields;
            uint32_t binding_index, binding_space;
        };
        struct SharedUniformBuffer : public UniformBuffer{
            SharedUniformBuffer(UniformBuffer&& other){
                this->name = std::move(other.name);
                this->size = std::move(other.size);
                this->fields = std::move(other.fields);
                this->binding_index = other.binding_index;
                this->binding_space = other.binding_space;
            }
            std::string index_name;
        };
        struct InstanceData {
            ShaderReflection::VertexBindingPoint bindingPoint;
            const ShaderReflection::Type* type;
        };
        struct LookupResult {
            size_t offset;
            size_t ubo_index;
            bool is_shared = false;
            bool is_instancedata = false;
            const ShaderReflection::Type* type;
        };
        std::optional<LookupResult> lookup(std::string_view path);
        ShaderReflection::Type* types;
        size_t types_size;
        std::vector<ModuleReflection::UniformBuffer> ubos;
        std::vector<ModuleReflection::SharedUniformBuffer> shared_ubos;
        //All vertex inputs except instance data
        std::vector<ShaderReflection::VertexBindingPoint> vertex_inputs;
        std::optional<InstanceData> instance_data;
    };
    class SlangLayer
    {
    public:
        enum class EntryPointType {
            VERTEX,
            FRAGMENT
        };

        static void Init();
        static Slang::ComPtr<slang::IModule> CompileModule(const std::string& filepath);
        static Slang::ComPtr<slang::IEntryPoint> GetModuleEntryPoint(Slang::ComPtr<slang::IModule> module, EntryPointType type);
        static Slang::ComPtr<slang::IComponentType> CreateCompositeComponentType(std::span<slang::IComponentType*> components);
        static Slang::ComPtr<slang::IComponentType> LinkModule(slang::IComponentType* component);
        static std::string GetModuleCode(slang::IComponentType* component, uint32_t entryPointIndex = 0);
        static ModuleReflection GetProgramReflection(Slang::ComPtr<slang::IComponentType> program);
        static void Shutdown();
    public:
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
    };
};