#pragma once

#include "MaterialCommentParser.hpp"
#include "Reflection.hpp"

#include <slang-com-ptr.h>
#include <slang.h>

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

inline bool readTextFile(const std::string& path, std::string& outSource)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    std::ostringstream buffer;
    buffer << file.rdbuf();
    outSource = buffer.str();
    return true;
}

struct PermutationChoice
{
    std::string name;
    std::string value;
};

struct CompiledEntryPoint
{
    std::string name;
    std::vector<uint8_t> code;
};

struct TargetCompileResult
{
    SlangCompileTarget format = SLANG_TARGET_UNKNOWN;
    std::vector<CompiledEntryPoint> entryPoints;
    std::vector<ReflectedResource> reflection;
};

struct CompileResult
{
    MaterialFileMeta meta;

    std::vector<PermutationChoice> macroChoices;
    std::vector<PermutationChoice> typeChoices;

    bool success = false;

    std::vector<TargetCompileResult> targets;

    std::string diagnostics;
};

class MaterialCompiler
{
public:
    MaterialCompiler(std::vector<SlangCompileTarget> targets,
                      std::vector<PermutationChoice> macroChoices = {},
                      std::vector<std::string> searchPaths = {})
        : mTargets(std::move(targets))
        , mMacroChoices(std::move(macroChoices))
    {
        if(sGlobalSession == nullptr) {
            if (SLANG_FAILED(slang::createGlobalSession(sGlobalSession.writeRef())))
            {
                mInitError = "failed to create Slang global session";
                return;
            }
        }

        std::vector<slang::TargetDesc> targetDescs;
        targetDescs.reserve(mTargets.size());
        for (SlangCompileTarget t : mTargets)
        {
            slang::TargetDesc desc;
            desc.format = t;
            targetDescs.push_back(desc);
        }

        mSearchPaths = std::move(searchPaths);
        std::vector<const char*> searchPathPtrs;
        searchPathPtrs.reserve(mSearchPaths.size());
        for (const std::string& p : mSearchPaths)
            searchPathPtrs.push_back(p.c_str());

        std::vector<slang::PreprocessorMacroDesc> macroDescs;
        macroDescs.reserve(mMacroChoices.size());
        for (const PermutationChoice& choice : mMacroChoices)
        {
            slang::PreprocessorMacroDesc desc;
            desc.name = choice.name.c_str();
            desc.value = choice.value.c_str();
            macroDescs.push_back(desc);
        }

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = targetDescs.data();
        sessionDesc.targetCount = static_cast<SlangInt>(targetDescs.size());
        sessionDesc.searchPaths = searchPathPtrs.data();
        sessionDesc.searchPathCount = static_cast<SlangInt>(searchPathPtrs.size());
        sessionDesc.preprocessorMacros = macroDescs.data();
        sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(macroDescs.size());

        if (SLANG_FAILED(sGlobalSession->createSession(sessionDesc, mSession.writeRef())))
        {
            mInitError = "failed to create Slang session";
        }
    }

    bool isValid() const { return mSession != nullptr; }
    const std::string& initError() const { return mInitError; }
    const std::vector<PermutationChoice>& macroChoices() const { return mMacroChoices; }

    CompileResult compileFile(const std::string& path, std::vector<PermutationChoice> typeChoices = {}) const
    {
        CompileResult result;
        result.macroChoices = mMacroChoices;
        result.typeChoices = typeChoices;

        if (!isValid())
        {
            result.diagnostics = mInitError;
            return result;
        }

        std::string source;
        if (!readTextFile(path, source))
        {
            result.diagnostics = "could not read file: " + path;
            return result;
        }
        return compileSource(path, source, std::move(typeChoices));
    }
    CompileResult compileSource(const std::string& path, const std::string& source, std::vector<PermutationChoice> typeChoices = {}) const
    {
        CompileResult result;
        result.macroChoices = mMacroChoices;
        result.typeChoices = typeChoices;

        if (!isValid())
        {
            result.diagnostics = mInitError;
            return result;
        }

        result.meta = parseMaterialComments(source);
        if (!result.meta.ok())
        {
            return result; // success stays false; caller reads meta.errors
        }

        return compileValidatedSource(path, source, std::move(result.meta),
                                       std::move(typeChoices));
    }
private:
    static std::string blobToString(slang::IBlob* blob)
    {
        if (!blob) return {};
        return std::string(static_cast<const char*>(blob->getBufferPointer()),
                            blob->getBufferSize());
    }

    static std::vector<uint8_t> blobToBytes(slang::IBlob* blob)
    {
        if (!blob) return {};
        const uint8_t* p = static_cast<const uint8_t*>(blob->getBufferPointer());
        return std::vector<uint8_t>(p, p + blob->getBufferSize());
    }

    CompileResult compileValidatedSource(const std::string& path,
                                          const std::string& source,
                                          MaterialFileMeta meta,
                                          std::vector<PermutationChoice> typeChoices) const
    {
        CompileResult result;
        result.meta = std::move(meta);
        result.macroChoices = mMacroChoices;
        result.typeChoices = typeChoices;

        std::string moduleName = result.meta.name.empty() ? path : result.meta.name;

        Slang::ComPtr<slang::IBlob> loadDiagnostics;
        slang::IModule* module = mSession->loadModuleFromSourceString(
            moduleName.c_str(), path.c_str(), source.c_str(), loadDiagnostics.writeRef());

        appendDiagnostics(result.diagnostics, loadDiagnostics);

        if (!module)
        {
            return result;
        }

        std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
        std::vector<std::string> entryPointNames;

        SlangInt32 entryPointCount = module->getDefinedEntryPointCount();
        for (SlangInt32 i = 0; i < entryPointCount; ++i)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint;
            if (SLANG_FAILED(module->getDefinedEntryPoint(i, entryPoint.writeRef())))
                continue;

            if (auto* fn = entryPoint->getFunctionReflection())
                entryPointNames.push_back(fn->getName());
            else
                entryPointNames.push_back("entryPoint" + std::to_string(i));

            entryPoints.push_back(std::move(entryPoint));
        }

        if (entryPoints.empty())
        {
            result.diagnostics += "no [shader(\"...\")] entry points found in " + path + "\n";
            return result;
        }

        std::vector<slang::IComponentType*> components;
        components.push_back(module);
        for (auto& ep : entryPoints)
            components.push_back(ep.get());

        Slang::ComPtr<slang::IComponentType> composite;
        Slang::ComPtr<slang::IBlob> compositeDiagnostics;
        if (SLANG_FAILED(mSession->createCompositeComponentType(
                components.data(),
                static_cast<SlangInt>(components.size()),
                composite.writeRef(),
                compositeDiagnostics.writeRef())))
        {
            appendDiagnostics(result.diagnostics, compositeDiagnostics);
            return result;
        }
        appendDiagnostics(result.diagnostics, compositeDiagnostics);

        Slang::ComPtr<slang::IComponentType> linked;
        Slang::ComPtr<slang::IBlob> linkDiagnostics;
        if (SLANG_FAILED(composite->link(linked.writeRef(), linkDiagnostics.writeRef())))
        {
            appendDiagnostics(result.diagnostics, linkDiagnostics);
            return result;
        }
        appendDiagnostics(result.diagnostics, linkDiagnostics);

        Slang::ComPtr<slang::IComponentType> compiledProgram = linked;

        if (!typeChoices.empty())
        {
            slang::ProgramLayout* layout = linked->getLayout();
            if (!layout)
            {
                result.diagnostics += "could not get program layout to resolve "
                                       "type permutation arguments for " + path + "\n";
                return result;
            }

            std::vector<slang::SpecializationArg> specArgs;
            specArgs.reserve(typeChoices.size());
            for (const PermutationChoice& choice : typeChoices)
            {
                slang::TypeReflection* type = layout->findTypeByName(choice.value.c_str());
                if (!type)
                {
                    result.diagnostics += "unknown type '" + choice.value +
                        "' for type permutation '" + choice.name + "' in " + path + "\n";
                    return result;
                }
                specArgs.push_back(slang::SpecializationArg::fromType(type));
            }

            Slang::ComPtr<slang::IComponentType> specialized;
            Slang::ComPtr<slang::IBlob> specializeDiagnostics;
            if (SLANG_FAILED(linked->specialize(
                    specArgs.data(),
                    static_cast<SlangInt>(specArgs.size()),
                    specialized.writeRef(),
                    specializeDiagnostics.writeRef())))
            {
                appendDiagnostics(result.diagnostics, specializeDiagnostics);
                return result;
            }
            appendDiagnostics(result.diagnostics, specializeDiagnostics);
            compiledProgram = specialized;
        }

        bool anyTargetFailed = false;
        for (size_t targetIndex = 0; targetIndex < mTargets.size(); ++targetIndex)
        {
            TargetCompileResult targetResult;
            targetResult.format = mTargets[targetIndex];
            targetResult.reflection = extractReflection(compiledProgram->getLayout());
            for (size_t epIndex = 0; epIndex < entryPoints.size(); ++epIndex)
            {
                Slang::ComPtr<slang::IBlob> code;
                Slang::ComPtr<slang::IBlob> codeDiagnostics;
                SlangResult codeResult = compiledProgram->getEntryPointCode(
                    static_cast<SlangInt>(epIndex),
                    static_cast<SlangInt>(targetIndex),
                    code.writeRef(),
                    codeDiagnostics.writeRef());

                appendDiagnostics(result.diagnostics, codeDiagnostics);

                if (SLANG_FAILED(codeResult))
                {
                    anyTargetFailed = true;
                    continue;
                }

                CompiledEntryPoint entry;
                entry.name = entryPointNames[epIndex];
                entry.code = blobToBytes(code);
                targetResult.entryPoints.push_back(std::move(entry));
            }

            result.targets.push_back(std::move(targetResult));
        }

        result.success = !anyTargetFailed;
        return result;
    }

    static void appendDiagnostics(std::string& out, slang::IBlob* diagnostics)
    {
        if (!diagnostics) return;
        std::string text = blobToString(diagnostics);
        if (!text.empty()) out += text;
    }

    Slang::ComPtr<slang::IGlobalSession> sGlobalSession = nullptr;
    Slang::ComPtr<slang::ISession> mSession;
    std::vector<SlangCompileTarget> mTargets;
    std::vector<PermutationChoice> mMacroChoices;
    std::vector<std::string> mSearchPaths;
    std::string mInitError;
};