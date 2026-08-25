#pragma once

#include "Compiler.hpp"
#include "MaterialCommentParser.hpp"

#include <string>
#include <vector>

struct VariantResult : public CompileResult
{
    std::string variantName;
};
struct BatchCompileResult
{
    MaterialFileMeta meta;

    std::vector<VariantResult> variants;

    bool success = false;
};


inline std::string makeVariantName(const std::vector<PermutationChoice>& macroChoices,
                                    const std::vector<PermutationChoice>& typeChoices)
{
    if (macroChoices.empty() && typeChoices.empty())
        return "default";

    std::string name;
    auto appendChoices = [&name](const std::vector<PermutationChoice>& choices)
    {
        for (const PermutationChoice& c : choices)
        {
            if (!name.empty()) name += ",";
            name += c.name + "=" + c.value;
        }
    };
    appendChoices(macroChoices);
    appendChoices(typeChoices);
    return name;
}

class BatchCompiler
{
public:
    BatchCompiler(std::vector<SlangCompileTarget> targets,
                   std::vector<std::string> searchPaths = {})
        : mTargets(std::move(targets))
        , mSearchPaths(std::move(searchPaths))
    {
    }

    BatchCompileResult compileFile(const std::string& path) const
    {
        BatchCompileResult batch;

        std::string source;
        if (!readTextFile(path, source))
        {
            batch.meta.errors.push_back({0, "", "could not read file: " + path});
            return batch;
        }

        return compileSource(path, source);
    }
    BatchCompileResult compileSource(const std::string& path, const std::string& source) const
    {
        BatchCompileResult batch;

        batch.meta = parseMaterialComments(source);
        if (!batch.meta.ok())
        {
            return batch;
        }

        std::vector<const PermutationDef*> macroDefs, typeDefs;
        for (const PermutationDef& p : batch.meta.permutations)
        {
            if (p.kind == PermutationKind::Macro) macroDefs.push_back(&p);
            else                                  typeDefs.push_back(&p);
        }

        std::vector<std::vector<PermutationChoice>> macroCombos = cartesianProduct(macroDefs);
        std::vector<std::vector<PermutationChoice>> typeCombos = cartesianProduct(typeDefs);

        batch.success = true;

        for (const std::vector<PermutationChoice>& macroCombo : macroCombos)
        {
            MaterialCompiler compiler(mTargets, macroCombo, mSearchPaths);

            for (const std::vector<PermutationChoice>& typeCombo : typeCombos)
            {
                VariantResult result = VariantResult(compiler.compileSource(path, source, typeCombo));
                result.variantName = makeVariantName(macroCombo, typeCombo);
                if (!result.success) batch.success = false;
                batch.variants.push_back(std::move(result));
            }
        }

        return batch;
    }
private:
    static std::vector<std::vector<PermutationChoice>> cartesianProduct(
        const std::vector<const PermutationDef*>& defs)
    {
        std::vector<std::vector<PermutationChoice>> combos;
        combos.emplace_back(); // seed with one empty combination

        for (const PermutationDef* def : defs)
        {
            std::vector<std::vector<PermutationChoice>> expanded;
            expanded.reserve(combos.size() * (def->values.empty() ? 1 : def->values.size()));

            for (const std::vector<PermutationChoice>& partial : combos)
            {
                if (def->values.empty())
                {
                    expanded.push_back(partial);
                    continue;
                }

                for (const std::string& value : def->values)
                {
                    std::vector<PermutationChoice> combo = partial;
                    combo.push_back({def->name, value});
                    expanded.push_back(std::move(combo));
                }
            }

            combos = std::move(expanded);
        }

        return combos;
    }

    std::vector<SlangCompileTarget> mTargets;
    std::vector<std::string> mSearchPaths;
};