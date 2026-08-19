// MaterialBinary.hpp
//
// On-disk .rshdr format. Layout, in file order:
//
//   [Header]        magic, version - nothing else.
//   [Material Info] name, domain, render state, tags.
//   [Variant Table] every variant enumerated ONCE: name + macro/type
//                   choices. Everything after this point refers to a
//                   variant by its index into this table, never by
//                   repeating its data.
//   [Target Table]  one entry per target: format + byte offset + byte
//                   size of that target's data block. The offset+size
//                   pair is what lets a reader skip a target block
//                   entirely without understanding its contents - add or
//                   remove a target type and every OTHER block's location
//                   is still exactly where its table entry says it is.
//   [Target Data]   one block per target (located via the target table,
//                   not by reading sequentially). Each block contains
//                   exactly variantCount entries, one per variant index
//                   in order - a target is never allowed to cover only
//                   some variants.
//
// The target table's offsets are placeholders at write time (the data
// they point to hasn't been written yet) and get patched in after each
// block is written - see BinaryIO.hpp's seek()/tell().

#pragma once

#include "BatchCompiler.hpp"
#include "BinaryIO.hpp"
#include "Compiler.hpp"
#include "MaterialCommentParser.hpp"

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
//The same as VK_MAKE_VERSION
#define MAKE_VERSION(major, minor, patch) \
    ((((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))

inline constexpr char kMaterialBinaryMagic[4] = {'R', 'S', 'H', 'D'};
inline constexpr uint32_t kMaterialBinaryVersion = MAKE_VERSION(0, 0, 1);

// -----------------------------------------------------------------------
// Loaded (post-read) data model - deliberately separate from
// MaterialFileMeta/CompileResult, which carry parse/compile-time-only
// concerns (ParseError, diagnostics, success flags) that have no meaning
// once the file is sitting on disk as validated, already-compiled output.
//
// This is variant-major (each variant owns its per-target entry points),
// which is the shape a runtime actually wants to query ("give me variant
// X's code for target Y") - the on-disk layout is target-major instead,
// for the skip-without-understanding property described above. Reading
// re-assembles one shape from the other; nothing about that is visible
// to callers of readMaterialBinary().
// -----------------------------------------------------------------------

struct LoadedEntryPoint
{
    std::string name;
    std::vector<uint8_t> code;
};

struct LoadedVariantTarget
{
    SlangCompileTarget format = SLANG_TARGET_UNKNOWN;
    std::vector<LoadedEntryPoint> entryPoints;
};

struct LoadedVariant
{
    std::string variantName;
    std::vector<PermutationChoice> macroChoices;
    std::vector<PermutationChoice> typeChoices;
    std::vector<LoadedVariantTarget> targets;
};

struct LoadedMaterial
{
    std::string name;
    std::string domain;

    // Matches MaterialFileMeta: blend/cull/depthTest always have a value
    // (the compiler assigns a default when a file doesn't state one), so
    // there's nothing to distinguish "unset" from here - only depthWrite
    // has no universal default and stays optional.
    BlendMode blend = BlendMode::Opaque;
    CullMode cull = CullMode::Back;
    DepthTestMode depthTest = DepthTestMode::Less;
    std::optional<bool> depthWrite;

    std::vector<MaterialTag> tags;
    std::vector<LoadedVariant> variants;
};

namespace detail
{
    template <typename Enum>
    inline void writeEnum(BinaryWriter& writer, Enum value)
    {
        writer.writeU8(static_cast<uint8_t>(value));
    }

    template <typename Enum>
    inline bool readEnum(BinaryReader& reader, Enum& value)
    {
        uint8_t raw = 0;
        if (!reader.readU8(raw)) return false;
        value = static_cast<Enum>(raw);
        return true;
    }

    template <typename Enum>
    inline void writeOptionalEnum(BinaryWriter& writer, const std::optional<Enum>& value)
    {
        writer.writeBool(value.has_value());
        if (value) writer.writeU8(static_cast<uint8_t>(*value));
    }

    template <typename Enum>
    inline bool readOptionalEnum(BinaryReader& reader, std::optional<Enum>& value)
    {
        bool hasValue = false;
        if (!reader.readBool(hasValue)) return false;
        if (!hasValue)
        {
            value.reset();
            return true;
        }
        uint8_t raw = 0;
        if (!reader.readU8(raw)) return false;
        value = static_cast<Enum>(raw);
        return true;
    }

    inline void writeChoices(BinaryWriter& writer, const std::vector<PermutationChoice>& choices)
    {
        writer.writeU32(static_cast<uint32_t>(choices.size()));
        for (const PermutationChoice& c : choices)
        {
            writer.writeString(c.name);
            writer.writeString(c.value);
        }
    }

    inline bool readChoices(BinaryReader& reader, std::vector<PermutationChoice>& choices)
    {
        uint32_t count = 0;
        if (!reader.readU32(count)) return false;
        choices.clear();
        choices.reserve(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            PermutationChoice c;
            if (!reader.readString(c.name) || !reader.readString(c.value)) return false;
            choices.push_back(std::move(c));
        }
        return true;
    }
}

// Refuses to write anything for a batch that didn't fully succeed
inline bool writeMaterialBinary(const std::string& path,
                                 const BatchCompileResult& batch,
                                 std::string* outError = nullptr)
{
    if (!batch.meta.ok())
    {
        if (outError) *outError = "refusing to write: material metadata has errors";
        return false;
    }
    if (!batch.success)
    {
        if (outError) *outError = "refusing to write: one or more variants failed to compile";
        return false;
    }

    BinaryWriter writer(path);
    if (!writer.isOpen())
    {
        if (outError) *outError = "could not open output file: " + path;
        return false;
    }

    // --- Header ---
    writer.writeBytes(kMaterialBinaryMagic, sizeof(kMaterialBinaryMagic));
    writer.writeU32(kMaterialBinaryVersion);

    // --- Material info ---
    writer.writeString(batch.meta.name);
    writer.writeString(batch.meta.domain);
    detail::writeEnum(writer, batch.meta.blend.value_or(kDefaultBlendMode));
    detail::writeEnum(writer, batch.meta.cull.value_or(kDefaultCullMode));
    detail::writeEnum(writer, batch.meta.depthTest.value_or(kDefaultDepthTestMode));
    detail::writeOptionalEnum(writer, batch.meta.depthWrite);

    writer.writeU32(static_cast<uint32_t>(batch.meta.tags.size()));
    for (const MaterialTag& tag : batch.meta.tags)
    {
        writer.writeString(tag.key);
        writer.writeString(tag.value);
    }

    // --- Variant table: enumerated once, referenced by index from here on ---
    const uint32_t variantCount = static_cast<uint32_t>(batch.variants.size());
    writer.writeU32(variantCount);
    for (const VariantResult& variant : batch.variants)
    {
        writer.writeString(variant.variantName);
        detail::writeChoices(writer, variant.macroChoices);
        detail::writeChoices(writer, variant.typeChoices);
    }

    for (size_t i = 1; i < batch.variants.size(); ++i)
    {
        if (batch.variants[i].targets.size() != batch.variants[0].targets.size())
        {
            if (outError) *outError = "refusing to write: variants have differing target counts";
            return false;
        }
        for (size_t t = 0; t < batch.variants[i].targets.size(); ++t)
        {
            if (batch.variants[i].targets[t].format != batch.variants[0].targets[t].format)
            {
                if (outError) *outError = "refusing to write: variants have differing target order/formats";
                return false;
            }
        }
    }

    const uint32_t targetCount =
        batch.variants.empty() ? 0 : static_cast<uint32_t>(batch.variants[0].targets.size());

    // --- Target table: format + offset + size, offset/size patched in below ---
    writer.writeU32(targetCount);
    const uint64_t targetTableStart = writer.tell();
    for (uint32_t t = 0; t < targetCount; ++t)
    {
        writer.writeU32(static_cast<uint32_t>(batch.variants[0].targets[t].format));
        writer.writeU64(0); // offset placeholder, patched after writing the block
        writer.writeU64(0); // size placeholder, patched after writing the block
    }

    // --- Target data: one block per target, variantCount entries each ---
    for (uint32_t t = 0; t < targetCount; ++t)
    {
        const uint64_t blockStart = writer.tell();

        for (uint32_t v = 0; v < variantCount; ++v)
        {
            const TargetCompileResult& targetResult = batch.variants[v].targets[t];

            writer.writeU32(v); // self-check: must equal this entry's position
            writer.writeU32(static_cast<uint32_t>(targetResult.entryPoints.size()));
            for (const CompiledEntryPoint& ep : targetResult.entryPoints)
            {
                writer.writeString(ep.name);
                writer.writeBytes(ep.code);
            }
        }

        const uint64_t blockEnd = writer.tell();

        // Patch this target's offset/size entry now that we know both.
        const uint64_t entryPos = targetTableStart +
            static_cast<uint64_t>(t) * (sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t));
        writer.seek(entryPos + sizeof(uint32_t)); // skip past the format field
        writer.writeU64(blockStart);
        writer.writeU64(blockEnd - blockStart);
        writer.seek(blockEnd); // resume appending where we left off
    }

    return writer.good();
}

inline bool readMaterialBinary(const std::string& path,
                                LoadedMaterial& outMaterial,
                                std::string* outError = nullptr)
{
    BinaryReader reader(path);
    if (!reader.isOpen())
    {
        if (outError) *outError = "could not open input file: " + path;
        return false;
    }

    // --- Header ---
    char magic[4] = {};
    if (!reader.readBytes(magic, sizeof(magic)) ||
        std::memcmp(magic, kMaterialBinaryMagic, sizeof(magic)) != 0)
    {
        if (outError) *outError = "not a material binary (bad magic): " + path;
        return false;
    }

    uint32_t version = 0;
    if (!reader.readU32(version))
    {
        if (outError) *outError = "truncated file (missing version): " + path;
        return false;
    }
    if (version != kMaterialBinaryVersion)
    {
        if (outError)
            *outError = "unsupported material binary version " + std::to_string(version) +
                         " (expected " + std::to_string(kMaterialBinaryVersion) + "): " + path;
        return false;
    }

    LoadedMaterial material;

    // --- Material info ---
    if (!reader.readString(material.name) || !reader.readString(material.domain))
    {
        if (outError) *outError = "truncated file (name/domain): " + path;
        return false;
    }
    if (!detail::readEnum(reader, material.blend) ||
        !detail::readEnum(reader, material.cull) ||
        !detail::readEnum(reader, material.depthTest) ||
        !detail::readOptionalEnum(reader, material.depthWrite))
    {
        if (outError) *outError = "truncated file (render state): " + path;
        return false;
    }

    uint32_t tagCount = 0;
    if (!reader.readU32(tagCount))
    {
        if (outError) *outError = "truncated file (tag count): " + path;
        return false;
    }
    material.tags.reserve(tagCount);
    for (uint32_t i = 0; i < tagCount; ++i)
    {
        MaterialTag tag;
        if (!reader.readString(tag.key) || !reader.readString(tag.value))
        {
            if (outError) *outError = "truncated file (tag " + std::to_string(i) + "): " + path;
            return false;
        }
        material.tags.push_back(std::move(tag));
    }

    // --- Variant table ---
    uint32_t variantCount = 0;
    if (!reader.readU32(variantCount))
    {
        if (outError) *outError = "truncated file (variant count): " + path;
        return false;
    }
    material.variants.resize(variantCount);
    for (uint32_t v = 0; v < variantCount; ++v)
    {
        if (!reader.readString(material.variants[v].variantName) ||
            !detail::readChoices(reader, material.variants[v].macroChoices) ||
            !detail::readChoices(reader, material.variants[v].typeChoices))
        {
            if (outError) *outError = "truncated file (variant " + std::to_string(v) + "): " + path;
            return false;
        }
    }

    // --- Target table ---
    uint32_t targetCount = 0;
    if (!reader.readU32(targetCount))
    {
        if (outError) *outError = "truncated file (target count): " + path;
        return false;
    }

    struct TargetTableEntry { SlangCompileTarget format; uint64_t offset; uint64_t size; };
    std::vector<TargetTableEntry> targetTable(targetCount);
    for (uint32_t t = 0; t < targetCount; ++t)
    {
        uint32_t formatRaw = 0;
        uint64_t offset = 0, size = 0;
        if (!reader.readU32(formatRaw) || !reader.readU64(offset) || !reader.readU64(size))
        {
            if (outError) *outError = "truncated file (target table entry " + std::to_string(t) + "): " + path;
            return false;
        }
        targetTable[t] = {static_cast<SlangCompileTarget>(formatRaw), offset, size};
    }

    // --- Target data blocks, visited via the table's offsets rather than
    //     read positionally, so an unrecognized future target format
    //     could be skipped by offset+size alone without any of this code
    //     needing to change. ---
    for (const TargetTableEntry& entry : targetTable)
    {
        reader.seek(entry.offset);

        for (uint32_t v = 0; v < variantCount; ++v)
        {
            uint32_t variantIndex = 0;
            uint32_t entryPointCount = 0;
            if (!reader.readU32(variantIndex) || !reader.readU32(entryPointCount))
            {
                if (outError) *outError = "truncated file (target block, variant " + std::to_string(v) + "): " + path;
                return false;
            }
            if (variantIndex != v)
            {
                if (outError)
                    *outError = "corrupt file: expected variant index " + std::to_string(v) +
                                 " but found " + std::to_string(variantIndex) + " in " + path;
                return false;
            }

            LoadedVariantTarget target;
            target.format = entry.format;
            target.entryPoints.reserve(entryPointCount);
            for (uint32_t e = 0; e < entryPointCount; ++e)
            {
                LoadedEntryPoint ep;
                if (!reader.readString(ep.name) || !reader.readBytes(ep.code))
                {
                    if (outError) *outError = "truncated file (entry point " + std::to_string(e) + "): " + path;
                    return false;
                }
                target.entryPoints.push_back(std::move(ep));
            }

            material.variants[v].targets.push_back(std::move(target));
        }
    }

    outMaterial = std::move(material);
    return true;
}

// Reads only ONE target's data - skips straight to it via the target
// table instead of reading the rest of the file. This is the actual
// payoff of storing offsets: a consumer that only cares about SPIR-V
// (say, a Vulkan-only runtime) never touches the DXIL block at all.
//
// Still needs the variant table (to know how many variants and what
// they're called), so this isn't free of a header read - just free of
// reading every OTHER target's bytecode.
inline bool readMaterialBinaryTarget(const std::string& path,
                                      SlangCompileTarget wantedFormat,
                                      std::vector<LoadedEntryPoint>& outPerVariantEntryPointsFlat,
                                      std::vector<std::string>& outVariantNames,
                                      std::string* outError = nullptr)
{
    BinaryReader reader(path);
    if (!reader.isOpen())
    {
        if (outError) *outError = "could not open input file: " + path;
        return false;
    }

    char magic[4] = {};
    if (!reader.readBytes(magic, sizeof(magic)) ||
        std::memcmp(magic, kMaterialBinaryMagic, sizeof(magic)) != 0)
    {
        if (outError) *outError = "not a material binary (bad magic): " + path;
        return false;
    }

    uint32_t version = 0;
    if (!reader.readU32(version) || version != kMaterialBinaryVersion)
    {
        if (outError) *outError = "unsupported/missing version in " + path;
        return false;
    }

    std::string name, domain;
    BlendMode blend;
    CullMode cull;
    DepthTestMode depthTest;
    std::optional<bool> depthWrite;
    if (!reader.readString(name) || !reader.readString(domain) ||
        !detail::readEnum(reader, blend) ||
        !detail::readEnum(reader, cull) ||
        !detail::readEnum(reader, depthTest) ||
        !detail::readOptionalEnum(reader, depthWrite))
    {
        if (outError) *outError = "truncated file (material info): " + path;
        return false;
    }

    uint32_t tagCount = 0;
    if (!reader.readU32(tagCount))
    {
        if (outError) *outError = "truncated file (tag count): " + path;
        return false;
    }
    for (uint32_t i = 0; i < tagCount; ++i)
    {
        std::string k, v;
        if (!reader.readString(k) || !reader.readString(v))
        {
            if (outError) *outError = "truncated file (tags): " + path;
            return false;
        }
    }

    uint32_t variantCount = 0;
    if (!reader.readU32(variantCount))
    {
        if (outError) *outError = "truncated file (variant count): " + path;
        return false;
    }
    outVariantNames.resize(variantCount);
    for (uint32_t v = 0; v < variantCount; ++v)
    {
        std::vector<PermutationChoice> macroChoices, typeChoices;
        if (!reader.readString(outVariantNames[v]) ||
            !detail::readChoices(reader, macroChoices) ||
            !detail::readChoices(reader, typeChoices))
        {
            if (outError) *outError = "truncated file (variant " + std::to_string(v) + "): " + path;
            return false;
        }
    }

    uint32_t targetCount = 0;
    if (!reader.readU32(targetCount))
    {
        if (outError) *outError = "truncated file (target count): " + path;
        return false;
    }

    uint64_t wantedOffset = 0;
    bool found = false;
    for (uint32_t t = 0; t < targetCount; ++t)
    {
        uint32_t formatRaw = 0;
        uint64_t offset = 0, size = 0;
        if (!reader.readU32(formatRaw) || !reader.readU64(offset) || !reader.readU64(size))
        {
            if (outError) *outError = "truncated file (target table): " + path;
            return false;
        }
        if (static_cast<SlangCompileTarget>(formatRaw) == wantedFormat)
        {
            wantedOffset = offset;
            found = true;
            // Deliberately keep scanning the rest of the table rather
            // than breaking early - reading a u32+u64+u64 per remaining
            // entry is cheap, and it keeps this loop identical in shape
            // to the one in readMaterialBinary() rather than introducing
            // a second early-exit path to keep in sync.
        }
    }

    if (!found)
    {
        if (outError) *outError = "target format not present in " + path;
        return false;
    }

    reader.seek(wantedOffset);
    outPerVariantEntryPointsFlat.clear();
    for (uint32_t v = 0; v < variantCount; ++v)
    {
        uint32_t variantIndex = 0;
        uint32_t entryPointCount = 0;
        if (!reader.readU32(variantIndex) || !reader.readU32(entryPointCount))
        {
            if (outError) *outError = "truncated file (target block, variant " + std::to_string(v) + "): " + path;
            return false;
        }
        if (variantIndex != v)
        {
            if (outError)
                *outError = "corrupt file: expected variant index " + std::to_string(v) +
                             " but found " + std::to_string(variantIndex) + " in " + path;
            return false;
        }

        for (uint32_t e = 0; e < entryPointCount; ++e)
        {
            LoadedEntryPoint ep;
            if (!reader.readString(ep.name) || !reader.readBytes(ep.code))
            {
                if (outError) *outError = "truncated file (entry point " + std::to_string(e) + "): " + path;
                return false;
            }
            outPerVariantEntryPointsFlat.push_back(std::move(ep));
        }
    }

    return true;
}