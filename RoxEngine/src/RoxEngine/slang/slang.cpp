#include "slang.h"
#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/filesystem/Filesystem.hpp"
#include <RoxEngine/slang/slang.hpp>
#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

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

    std::optional<size_t> ModuleReflection::lookup(std::string_view path) {
        auto lookupUbo = [](std::string_view name,const ShaderReflection::Type** currentField,size_t& offset, std::vector<ModuleReflection::UniformBuffer>& ubos) -> ModuleReflection::UniformBuffer*{
            for (auto it = ubos.begin(); it != ubos.end(); ++it)
            {
                if (it->name == name)
                    return &(*it);
                for(auto field = it->fields.begin(); field != it->fields.end(); ++field) {
                    if(field->name == name) {
                        offset+=field->offset;
                        *currentField = field->type;
                        return &(*it);
                    }
                }
            }
            return nullptr;
        };
        auto lookupFieldUbo = [](std::string_view name, ModuleReflection::UniformBuffer* ubo, size_t& offset) -> const ShaderReflection::Type*{
            for(auto it = ubo->fields.begin(); it != ubo->fields.end(); ++it) {
                if(it->name == name) {
                    offset+=it->offset;
                    return it->type;
                }
            }
            return nullptr;
        };
        auto lookupField = [](std::string_view name, const ShaderReflection::Type* type, size_t& offset) -> const ShaderReflection::Type*{
            for(auto& field: type->fields) {
                if(field.first == name) {
                    offset += field.second->size;
                    return field.second;
                }
            }
            return nullptr;
        };
        auto parseIndex = [](std::string_view& str, uint32_t& outIndex) {
            if (str.empty() || str[0] != '[') return false;
            str.remove_prefix(1);  // skip '['

            size_t i = 0;
            while (i < str.size() && std::isdigit(str[i])) i++;
            if (i == 0 || i >= str.size() || str[i] != ']') return false;

            std::string_view number = str.substr(0, i);
            str.remove_prefix(i);
            outIndex = 0;
            for (char c : number)
                outIndex = outIndex * 10 + (c - '0');
            if (str.front() != ']') {
                return false;
            }
            str.remove_prefix(1);  // skip digits + ']'
            return true;
        };
        auto getMaxDimensions = [](const ShaderReflection::Type* type){
            auto count = 0;
            while (type) {
                switch(type->kind) {
                case ShaderReflection::Type::Scalar:
                    return count;
                case ShaderReflection::Type::Struct:
                case ShaderReflection::Type::Unknown:
                    return 0;
                case ShaderReflection::Type::Vector:
                    return 1;
                case ShaderReflection::Type::Matrix:
                    return 2;
                case ShaderReflection::Type::Array:
                    type = type->innerType;
                    count++;
                }
            }
            return count;
        };
        auto supportsIndex = [](ShaderReflection::Type::TypeKind kind){
            switch(kind) {
            case ShaderReflection::Type::Scalar:
            case ShaderReflection::Type::Struct:
            case ShaderReflection::Type::Unknown:
                return false;
            case ShaderReflection::Type::Vector:
            case ShaderReflection::Type::Matrix:
            case ShaderReflection::Type::Array:
                return true;
            }
        };

        std::string_view segment;
        std::string_view dimensions;
        ModuleReflection::UniformBuffer* ubo = nullptr;
        const ShaderReflection::Type* currentField = nullptr;
        size_t offset = 0;
        while(!path.empty()) {
            auto it =  path.find('.');
            segment = path.substr(0,it);
            path.remove_prefix(std::min(it,path.size()) );
            if(auto begin_dimensions = segment.find('['); begin_dimensions != std::string::npos) {
                dimensions = segment.substr(begin_dimensions);
                segment = segment.substr(0, begin_dimensions);
            }
            if(!ubo) {
                ubo = lookupUbo(segment, &currentField,offset, ubos);
                if(!ubo) {
                    return std::nullopt;
                }
            } else if(!currentField && ubo) {
                currentField = lookupFieldUbo(segment, ubo, offset);
                if(!currentField) {
                    return std::nullopt;
                }
            } else {
                currentField = lookupField(segment, currentField, offset);
                if(!currentField) {
                    return std::nullopt;
                }
            }
            
            if(!supportsIndex(currentField->kind) && !dimensions.empty()) {
                return std::nullopt;
            }
            uint32_t maxIndexes = getMaxDimensions(currentField);
            uint32_t currentDimension = 0;
            const ShaderReflection::Type* currentArrayField = currentField;
            while(!dimensions.empty()) {
                if(currentDimension > maxIndexes)
                    return std::nullopt;
                uint32_t index;
                if(!parseIndex(dimensions, index))
                    return std::nullopt;
                uint32_t maxIndex = 0;
                switch (currentArrayField->kind) {
                    case ShaderReflection::Type::TypeKind::Array:
                        maxIndex = currentArrayField->rowCount;
                        break;
                    case ShaderReflection::Type::TypeKind::Vector:
                        maxIndex = currentArrayField->rowCount;
                        break;
                    case ShaderReflection::Type::TypeKind::Matrix:
                        if(currentDimension == 0) {
                            maxIndex = currentArrayField->colCount;
                        } else {
                            maxIndex = currentArrayField->rowCount;
                        } 
                        break;
                    default:
                        return std::nullopt;
                }
                if (index >= maxIndex) {
                    return std::nullopt;
                }

                if(currentArrayField->kind == ShaderReflection::Type::TypeKind::Matrix) {
                    if(currentDimension == 0) {
                        offset += currentArrayField->stride * index;
                    } else {
                        offset += currentArrayField->innerType->size * index;
                    }
                } else {
                    offset += currentArrayField->stride * index;
                }
                if(currentArrayField->kind == ShaderReflection::Type::TypeKind::Array)
                    currentArrayField = currentArrayField->innerType;
                currentDimension++;
            }

        }
        return offset;
    }

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
    const ShaderReflection::Type* ExtractTypeRecursive(
    slang::TypeReflection* slangType,
    slang::TypeLayoutReflection* slangTypeLayout,
    std::unordered_set<ShaderReflection::Type>& typeSet) 
    {
        auto getScalarTypeSize = [](slang::TypeReflection* type) -> size_t {
            using namespace slang;

            switch (type->getScalarType())
            {
                case slang::TypeReflection::None:
                    return 0;
                case slang::TypeReflection::Void:
                    return 0;
                case slang::TypeReflection::Bool:
                    return 4;
                case slang::TypeReflection::Int32:
                    return 4;
                case slang::TypeReflection::UInt32:
                    return 4;
                case slang::TypeReflection::Int64:
                    return 8;
                case slang::TypeReflection::UInt64:
                    return 8;
                case slang::TypeReflection::Float16:
                    return 2;
                case slang::TypeReflection::Float32:
                    return 4;
                case slang::TypeReflection::Float64:
                    return 8;
                case slang::TypeReflection::Int8:
                    return 1;
                case slang::TypeReflection::UInt8:
                    return 1;
                case slang::TypeReflection::Int16:
                    return 2;
                case slang::TypeReflection::UInt16:
                    return 2;
            }
        };
        ShaderReflection::Type t;
        t.kind = ShaderReflection::Type::TypeKind::Unknown;
        ISlangBlob* nameBlob = nullptr;
        if(SLANG_SUCCEEDED(slangType->getFullName(&nameBlob))) {
            const char* data=(const char*)nameBlob->getBufferPointer();
            size_t size = nameBlob->getBufferSize();
            if(size > 0)
                t.name.assign(data);
            nameBlob->release();
        }

        using Kind = slang::TypeReflection::Kind;
        Kind k = slangType->getKind();

        switch (k) {
            case Kind::Scalar:
                t.kind = ShaderReflection::Type::TypeKind::Scalar;
                t.colCount = 1;
                t.rowCount = 1;
                t.size = getScalarTypeSize(slangType);
                t.stride = t.size;
                break;
            case Kind::Vector: {
                t.kind = ShaderReflection::Type::TypeKind::Vector;
                t.rowCount = slangType->getElementCount();
                t.colCount = 1;
                t.size = slangTypeLayout->getSize();
                t.innerType = ExtractTypeRecursive(slangType->getElementType(), slangTypeLayout->getElementTypeLayout(), typeSet);
                t.stride = slangTypeLayout->getElementTypeLayout()->getStride();
                break;
            }
            case Kind::Matrix: {
                t.kind = ShaderReflection::Type::TypeKind::Matrix;
                t.colCount = slangType->getColumnCount();
                t.rowCount = slangType->getRowCount();
                t.size = slangTypeLayout->getSize();
                t.innerType = ExtractTypeRecursive(slangType->getElementType(), slangTypeLayout->getElementTypeLayout(), typeSet);
                switch (slangTypeLayout->getMatrixLayoutMode()) {
                case SLANG_MATRIX_LAYOUT_MODE_UNKNOWN:
                    //Resort to this method if layout mode is unknown
                    t.stride = slangTypeLayout->getElementTypeLayout()->getStride();
                    break;
                case SLANG_MATRIX_LAYOUT_ROW_MAJOR:
                    t.stride = t.size / t.rowCount;
                    break;
                case SLANG_MATRIX_LAYOUT_COLUMN_MAJOR:
                    t.stride = t.size / t.colCount;
                    break;
                }
                break;
            }
            case Kind::Array: {
                t.kind = ShaderReflection::Type::TypeKind::Array;
                t.rowCount = slangType->getElementCount();
                t.size = slangTypeLayout->getSize();
                t.innerType = ExtractTypeRecursive(slangType->getElementType(), slangTypeLayout->getElementTypeLayout(), typeSet);
                //t.stride = slangTypeLayout->getElementTypeLayout()->getStride();
                //FIXME: try to not do this and use the api. This is here because of std140 errors
                t.stride = slangTypeLayout->getStride() / t.rowCount;
                break;
            }

            case Kind::Struct: {
                t.kind = ShaderReflection::Type::TypeKind::Struct;
                int fieldCount = slangType->getFieldCount();
                for (int i = 0; i < fieldCount; i++) {
                    slang::VariableReflection* fieldVar = slangType->getFieldByIndex(i);
                    auto fieldVarLayout = slangTypeLayout->getFieldByIndex(i);
                    const ShaderReflection::Type* inner = ExtractTypeRecursive(fieldVar->getType(),fieldVarLayout->getTypeLayout(), typeSet);
                    t.fields.emplace_back(fieldVar->getName(), inner);
                }
                break;
            }

            default:
                t.kind = ShaderReflection::Type::TypeKind::Unknown;
                break;
        }
        auto [it, inserted] = typeSet.insert(std::move(t));
        return &(*it);
    }
    ModuleReflection SlangLayer::GetModuleReflection(Slang::ComPtr<slang::IModule> module) {
        auto walkContantBuffer = [](slang::VariableLayoutReflection* cbuffer, std::unordered_set<ShaderReflection::Type>& typeSet){
            auto innerTypeLayout = cbuffer->getTypeLayout()->getElementTypeLayout();
            auto innerType = cbuffer->getTypeLayout()->getType()->getElementType();

            auto type_name = innerType->getName();
            ModuleReflection::UniformBuffer ubo;
            for(unsigned int i = 0; i < innerType->getFieldCount(); i++) {
                auto field = innerType->getFieldByIndex(i);
                auto var_layout = innerTypeLayout->getFieldByIndex(i);
                auto field_layout = var_layout->getTypeLayout();
                auto reflectionType = ExtractTypeRecursive(field->getType(),field_layout, typeSet);
                ubo.fields.push_back({field->getName(),var_layout->getOffset(),reflectionType});
            }
            return ubo;
        };

        auto layout = module->getLayout();
        std::unordered_set<ShaderReflection::Type> typeSet;
        std::vector<ModuleReflection::UniformBuffer> ubos;
        for (int i = 0; i < layout->getParameterCount(); i++)
        {
            slang::VariableLayoutReflection* var = layout->getParameterByIndex(i);
            if (var->getType()->getKind() != slang::TypeReflection::Kind::ConstantBuffer)
                continue;
            auto ubo = walkContantBuffer(var, typeSet);
            if(auto semanticName = var->getSemanticName();semanticName) {
                ubo.name = semanticName;
            } 
            ubos.push_back(ubo);
        }


        return {
            std::move(typeSet),
            std::move(ubos)
        };
    }
    std::string SlangLayer::LinkModules(std::span<Slang::ComPtr<slang::IModule>> modules) {
        if(modules.size() == 0) {
            return "";
        }
        Slang::ComPtr<slang::IEntryPoint> vertex_entrypoint = nullptr;
        Slang::ComPtr<slang::IEntryPoint> fragment_entrypoint = nullptr;

        for(auto& module : modules) {
            //TODO: get the chosen entry point from the main module (modules[0])
            if(!vertex_entrypoint)
                module->findEntryPointByName("basic_vmain", vertex_entrypoint.writeRef());
            if(!fragment_entrypoint)
                module->findEntryPointByName("basic_fmain", fragment_entrypoint.writeRef());

            if(vertex_entrypoint && fragment_entrypoint)
                break;
        }

        std::vector<slang::IComponentType*> components(modules.size() + 2);
        for(int i = 0; i < modules.size(); i++) {
            components[i] = modules[i];
        }
        components[components.size()-2] = vertex_entrypoint;
        components[components.size()-1] = fragment_entrypoint;

        Slang::ComPtr<slang::IComponentType> program;
        sSession->createCompositeComponentType(components.data(), components.size(), program.writeRef());
        Slang::ComPtr<slang::IBlob> diagnosticBlob = nullptr;
        Slang::ComPtr<slang::IComponentType> linkedProgram = nullptr;
        program->link(linkedProgram.writeRef(), diagnosticBlob.writeRef());

        if (diagnosticBlob)
        {
            log::error("Failed to link the program: {}", static_cast<const char*>(diagnosticBlob->getBufferPointer()));
            return 0;
        }

        Slang::ComPtr<slang::IBlob> vertex_diagnostic, fragment_diagnostic;
        Slang::ComPtr<slang::IBlob> vertex_code, fragment_code;


        linkedProgram->getEntryPointCode(0, 0, vertex_code.writeRef(), vertex_diagnostic.writeRef());
        linkedProgram->getEntryPointCode(1, 0, fragment_code.writeRef(), fragment_diagnostic.writeRef());

        if (vertex_diagnostic || fragment_diagnostic)
        {
            if (vertex_diagnostic) 
                log::error("Vertex shader error: {}", static_cast<const char*>(vertex_diagnostic->getBufferPointer()));
            if (fragment_diagnostic)
                log::error("Fragment shader error: {}", static_cast<const char*>(fragment_diagnostic->getBufferPointer()));
            return 0;
        }

        std::stringstream stream;
        stream << "  #define VERTEX_SHADER\n"; //2 spaces at the beggining to be possible to make the line a comment
        stream << "#ifdef VERTEX_SHADER\n";
        stream << (const char*)vertex_code->getBufferPointer();
        stream << "#else\n";
        stream << (const char*)fragment_code->getBufferPointer();
        stream << "#endif";
        

        return stream.str();
    }
    void SlangLayer::Shutdown()
    {
        sSession->release();
        sGlobalSession->release();
    }
}
