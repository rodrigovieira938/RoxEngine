#include "RoxEngine/core/Logger.hpp"
#include <cstring>
#include <iostream>
#include <glad/gl.h>
#include <slang/include/slang-com-ptr.h>
#include <slang/include/slang.h>
#include <string>
#include <RoxEngine/platforms/GL/GLMaterial.hpp>
#include <RoxEngine/platforms/GL/GLShader.hpp>
#include <RoxEngine/platforms/GL/GLUniformBuffer.hpp>
#include <RoxEngine/slang/slang.hpp>

namespace RoxEngine::GL {
    void compileErrors(unsigned int shader, const char* type)
    {
        // Stores status of compilation
        GLint hasCompiled;
        // Character array to store error message in
        char infoLog[1024];
        if (std::strcmp(type, "PROGRAM") != 0)
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                log::error("Shader Compilation error for {} : {}", type, infoLog);
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                log::error("Shader Linking error for {} : {}", type, infoLog);
            }
        }
    }
    GLuint CreateProgram(const char* vertexSource, const char* fragmentSource) {
        const GLuint id = glCreateProgram();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
        compileErrors(vertexShader, "VERTEX");
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);
        compileErrors(vertexShader, "VERTEX");

        glAttachShader(id, vertexShader);
        glAttachShader(id, fragmentShader);
        glLinkProgram(id);
        compileErrors(id, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return id;
    }


    GLuint CompileModule(const Slang::ComPtr<slang::IModule>& module, const Material::EntryPointInfo& entry_point)
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
        SlangLayer::sSession->createCompositeComponentType(components, 3, program.writeRef());
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

        return CreateProgram(
            static_cast<const char*>(vertex_code->getBufferPointer()),
            static_cast<const char*>(fragment_code->getBufferPointer()));
    }
    UniformBuffer::DataType::Kind slang_scalar_type_to_kind(slang::TypeReflection::ScalarType type) {
        switch (type)
        {
        case slang::TypeReflection::None:
        case slang::TypeReflection::Void:
        case slang::TypeReflection::Int64:
        case slang::TypeReflection::UInt64:
            break; //Unsupported or unneded types
        case slang::TypeReflection::Bool: //In gl bools are int32;
        case slang::TypeReflection::Float16:
            return UniformBuffer::DataType::Kind::HALF;
        case slang::TypeReflection::Float32:
            return UniformBuffer::DataType::Kind::FLOAT;
        case slang::TypeReflection::Float64:
            return UniformBuffer::DataType::Kind::DOUBLE;
        case slang::TypeReflection::Int8:
            return UniformBuffer::DataType::Kind::INT8;
        case slang::TypeReflection::UInt8:
            return UniformBuffer::DataType::Kind::UINT8;
        case slang::TypeReflection::Int16:
            return UniformBuffer::DataType::Kind::INT16;
        case slang::TypeReflection::UInt16:
            return UniformBuffer::DataType::Kind::UINT16;
        case slang::TypeReflection::Int32:
            return UniformBuffer::DataType::Kind::INT32;
        case slang::TypeReflection::UInt32:
            return UniformBuffer::DataType::Kind::UINT32;
        }
        return UniformBuffer::DataType::Kind::INT32;
    };

    void BakeScalar(const std::string& parent, std::unordered_map<std::string, UniformBuffer::DataType>& desc, slang::ProgramLayout* layout, slang::VariableReflection* field, slang::VariableLayoutReflection* field_layout)
    {
        slang::TypeReflection* field_type_reflection = field->getType();
        slang::TypeLayoutReflection* field_type_layout = field_layout->getTypeLayout();
        slang::TypeReflection::ScalarType scalar_type = field_type_reflection->getScalarType();
        char const* name = field->getName();
        desc[parent.empty() ? name : parent + "." + name] = {
            slang_scalar_type_to_kind(scalar_type),
            field_layout->getOffset()
        };
    }
    void BakeArray(const std::string& parent, std::unordered_map<std::string, UniformBuffer::DataType>& desc, slang::ProgramLayout* layout, slang::VariableReflection* field, slang::VariableLayoutReflection* field_layout)
    {
        slang::TypeReflection* field_type_reflection = field->getType();
        slang::TypeLayoutReflection* field_type_layout = field_layout->getTypeLayout();
        auto element_type = field_type_reflection->getElementType();
        auto element_type_layout = field_type_layout->getElementTypeLayout();
        std::vector<size_t> dimensions = { field_type_reflection->getElementCount() };
        while (element_type->getKind() == slang::TypeReflection::Kind::Array)
        {
            dimensions.push_back(element_type->getElementCount());
            element_type = element_type->getElementType();
            element_type_layout = field_type_layout->getElementTypeLayout();
        }
        auto scalarType = element_type->getScalarType();
        size_t alignment = static_cast<size_t>(field_type_layout->getAlignment());
        size_t size = element_type_layout->getSize();
        size_t truesize = size;
        size_t a = field_type_layout->getSize();
        if (alignment != 0)
            truesize = (size + alignment - 1) & ~(alignment - 1);

        UniformBuffer::DataType elementDataType = {
            (scalarType != slang::TypeReflection::None) ? slang_scalar_type_to_kind(scalarType) : UniformBuffer::DataType::Kind::STRUCT,
            static_cast<size_t>(field_layout->getOffset()),
            0,
            truesize,
        };

        desc[field->getName()] = {
            UniformBuffer::DataType::Kind::ARRAY,
            static_cast<size_t>(field_layout->getOffset()),
            0,
            truesize,
            (scalarType != slang::TypeReflection::None) ? slang_scalar_type_to_kind(scalarType) : UniformBuffer::DataType::Kind::STRUCT,
            static_cast<uint32_t>(dimensions[0])
        };
        for (size_t level = 0; level < dimensions.size(); level++) {
            size_t innerTotalLevels = dimensions.size() - level;
            size_t numCombinations = 1;


            auto datatype = elementDataType;
            if (level > 0)
            {
                datatype.innerKind = datatype.kind;
                datatype.kind = RoxEngine::UniformBuffer::DataType::ARRAY;
                datatype.rowsize = static_cast<uint32_t>(dimensions[dimensions.size() - level]);
            }

            for (size_t i = 0; i < innerTotalLevels; ++i) {
                numCombinations *= dimensions[i];
            }

            std::vector<size_t> currentCombination(innerTotalLevels, 0);

            for (size_t comb = 0; comb < numCombinations; ++comb) {
                std::string name = field->getName();
                for (size_t i = 0; i < innerTotalLevels; ++i) {
                    name += "[" + std::to_string(currentCombination[i]) + "]";
                }

                desc[name] = datatype;
                datatype.offset += datatype.item_offset;
                //Todo: add struct fields\

                for (size_t i = innerTotalLevels; i > 0; --i) {
                    if (currentCombination[i - 1]++ < dimensions[i - 1] - 1) {
                        break;
                    }
                    currentCombination[i - 1] = 0;
                }
            }
        }
    }
    void BakeMatrix(const std::string& parent, std::unordered_map<std::string, UniformBuffer::DataType>& desc, slang::ProgramLayout* layout, slang::VariableReflection* field, slang::VariableLayoutReflection* field_layout)
    {
        slang::TypeReflection* field_type_reflection = field->getType();
        slang::TypeLayoutReflection* field_type_layout = field_layout->getTypeLayout();
        auto element_type = field_type_reflection->getElementType();
        auto scalar_type = element_type->getScalarType();
        if (scalar_type != slang::TypeReflection::None)
        {
            desc[field->getName()] = {
                UniformBuffer::DataType::Kind::MATRIX,
                field_layout->getOffset(),
                static_cast<size_t>(field_type_layout->getAlignment()),
                0,
                slang_scalar_type_to_kind(scalar_type),
                static_cast<uint32_t>(field_type_reflection->getRowCount()),
                static_cast<uint32_t>(field_type_reflection->getColumnCount())
            };
        }
    }
    void BakeVector(const std::string& parent, std::unordered_map<std::string, UniformBuffer::DataType>& desc, slang::ProgramLayout* layout, slang::VariableReflection* field, slang::VariableLayoutReflection* field_layout)
    {
        slang::TypeReflection* field_type_reflection = field->getType();
        slang::TypeLayoutReflection* field_type_layout = field_layout->getTypeLayout();

        auto element_type = field_type_reflection->getElementType();
        auto scalar_type = element_type->getScalarType();
        if (scalar_type != slang::TypeReflection::None)
        {
            desc[field->getName()] = {
                UniformBuffer::DataType::Kind::VECTOR,
                field_layout->getOffset(),
                static_cast<size_t>(field_type_layout->getAlignment()),
                0,
                slang_scalar_type_to_kind(scalar_type),
                static_cast<uint32_t>(field_type_reflection->getElementCount()),
            };
        }
    }
    void BakeReflection(const std::string& parent, std::unordered_map<std::string, UniformBuffer::DataType>& desc, slang::ProgramLayout* layout, slang::VariableReflection* field, slang::VariableLayoutReflection* field_layout)
    {
        slang::TypeReflection* field_type_reflection = field->getType();
        slang::TypeLayoutReflection* field_type_layout = field_layout->getTypeLayout();
        slang::TypeReflection::Kind kind = field_type_reflection->getKind();
        //TODO: add support for structs
        //TODO: use a switch instead of ifs

        switch (kind)
        {

        case slang::TypeReflection::Kind::Struct:
            break;
        case slang::TypeReflection::Kind::Array:
            BakeArray("", desc, layout, field, field_layout);
            break;
        case slang::TypeReflection::Kind::Matrix:
            BakeMatrix("", desc, layout, field, field_layout);
            break;
        case slang::TypeReflection::Kind::Vector:
            BakeVector("", desc, layout, field, field_layout);
            break;
        case slang::TypeReflection::Kind::Scalar:
            BakeScalar("", desc, layout, field, field_layout);
            break;
        case slang::TypeReflection::Kind::None:
        case slang::TypeReflection::Kind::ConstantBuffer:
        case slang::TypeReflection::Kind::Resource:
        case slang::TypeReflection::Kind::SamplerState:
        case slang::TypeReflection::Kind::TextureBuffer:
        case slang::TypeReflection::Kind::ShaderStorageBuffer:
        case slang::TypeReflection::Kind::ParameterBlock:
        case slang::TypeReflection::Kind::GenericTypeParameter:
        case slang::TypeReflection::Kind::Interface:
        case slang::TypeReflection::Kind::OutputStream:
        case slang::TypeReflection::Kind::Specialized:
        case slang::TypeReflection::Kind::Feedback:
        case slang::TypeReflection::Kind::Pointer:
            break;
        }
    }


	Material::Material(const Ref<::RoxEngine::Shader>& shader, const EntryPointInfo& info, const char* link_time_constants)
	{
        auto module = std::static_pointer_cast<Shader>(shader)->mModule;
        auto layout = module->getLayout();
        mID = CompileModule(module, info);
        mShader = shader;
        for (int i = 0; i < layout->getParameterCount(); i++)
        {
            slang::VariableLayoutReflection* var = layout->getParameterByIndex(i);
            slang::TypeLayoutReflection* type_layout = var->getTypeLayout();
            slang::TypeReflection* type_reflection = var->getType();

            //TODO: support more types of buffers
            if (type_reflection->getKind() != slang::TypeReflection::Kind::ConstantBuffer)
                continue;
            std::unordered_map<std::string, UniformBuffer::DataType> desc;
            auto inner_type = type_reflection->getElementType();
            auto inner_type_layout = type_layout->getElementTypeLayout();
            for (int x = 0; x < inner_type->getFieldCount(); x++)
            {
                slang::VariableReflection* field = inner_type->getFieldByIndex(x);
                slang::VariableLayoutReflection* field_layout = inner_type_layout->getFieldByIndex(x);
                BakeReflection("", desc, layout, field, field_layout);
            }
            mUboMap[var->getName()] = mUbos.size();
            mUbos.emplace_back(CreateRef<UniformBuffer>(inner_type_layout->getSize(), var->getBindingIndex(), std::move(desc)));
        }
	}

	std::vector<Ref<RoxEngine::UniformBuffer>>& Material::GetUbos()
	{
        return mUbos;
	}

	Ref<RoxEngine::UniformBuffer> Material::GetUbo(const std::string& name)
	{
        auto it = mUboMap.find(name);
        if (it != mUboMap.end())
            return mUbos[it->second];
        return nullptr;
	}

	Ref<RoxEngine::Shader> Material::GetShader()
	{
        return mShader;
	}
}
