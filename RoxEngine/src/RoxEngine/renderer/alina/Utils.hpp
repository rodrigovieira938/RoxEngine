#pragma once
#include <alina/alina.hpp>

namespace std {
    // Helper to combine hashes
    inline void hash_combine(std::size_t& seed, std::size_t value) {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }
}

namespace alina {
    // ----------------------
    // Equality operators
    // ----------------------
    inline bool operator==(const BufferDesc& lhs, const BufferDesc& rhs) {
        return lhs.name == rhs.name &&
               lhs.type == rhs.type;
    }

    inline bool operator==(const GraphicsPipelineDesc& lhs, const GraphicsPipelineDesc& rhs) {
        return lhs.primType == rhs.primType &&
               lhs.renderState.rasterState.fillMode == rhs.renderState.rasterState.fillMode &&
               lhs.renderState.rasterState.cullMode == rhs.renderState.rasterState.cullMode &&
               lhs.inputLayout == rhs.inputLayout &&
               lhs.vs == rhs.vs &&
               lhs.fs == rhs.fs;
    }

    inline bool operator==(const SamplerDesc& lhs, const SamplerDesc& rhs) {
        return lhs.filter == rhs.filter &&
               lhs.minFilter == rhs.minFilter &&
               lhs.magFilter == rhs.magFilter &&
               lhs.mipFilter == rhs.mipFilter &&
               lhs.u == rhs.u && lhs.v == rhs.v && lhs.w == rhs.w &&
               lhs.border.r == rhs.border.r &&
               lhs.border.g == rhs.border.g &&
               lhs.border.b == rhs.border.b &&
               lhs.border.a == rhs.border.a;
    }

    inline bool operator==(const TextureDesc& lhs, const TextureDesc& rhs) {
        return lhs.format == rhs.format &&
               lhs.width == rhs.width &&
               lhs.height == rhs.height &&
               lhs.depth == rhs.depth &&
               lhs.mipLevels == rhs.mipLevels &&
               lhs.usage == rhs.usage &&
               lhs.name == rhs.name &&
               lhs.sampler == rhs.sampler;
    }
    inline bool operator==(const FramebufferAttachment& lhs, const FramebufferAttachment& rhs) {
            return lhs.texture == lhs.texture;
    }
    inline bool operator==(const FramebufferDesc& lhs, const FramebufferDesc& rhs) {
        return lhs.colorAttachments == rhs.colorAttachments &&
               lhs.depthAttachment.texture == rhs.depthAttachment.texture &&
               lhs.name == rhs.name;
    }

    inline bool operator==(const VertexAttributeDesc& lhs, const VertexAttributeDesc& rhs) {
        return lhs.name == rhs.name &&
               lhs.bufferIndex == rhs.bufferIndex &&
               lhs.arraySize == rhs.arraySize &&
               lhs.offset == rhs.offset &&
               lhs.stride == rhs.stride &&
               lhs.isInstanced == rhs.isInstanced &&
               lhs.format == rhs.format;
    }
}

// ----------------------
// Hash specializations
// ----------------------
namespace std {
    template<> struct hash<alina::BufferDesc> {
        size_t operator()(const alina::BufferDesc& d) const noexcept {
            size_t seed = 0;
            hash_combine(seed, std::hash<std::string>{}(d.name));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.type)));
            return seed;
        }
    };

    template<> struct hash<alina::GraphicsPipelineDesc> {
        size_t operator()(const alina::GraphicsPipelineDesc& d) const noexcept {
            size_t seed = 0;
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.primType)));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.renderState.rasterState.fillMode)));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.renderState.rasterState.cullMode)));
            hash_combine(seed, std::hash<void*>{}(d.inputLayout.get()));
            hash_combine(seed, std::hash<void*>{}(d.vs.get()));
            hash_combine(seed, std::hash<void*>{}(d.fs.get()));
            return seed;
        }
    };

    template<> struct hash<alina::SamplerDesc> {
        size_t operator()(const alina::SamplerDesc& d) const noexcept {
            size_t seed = 0;
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.filter)));
            hash_combine(seed, std::hash<bool>{}(d.minFilter));
            hash_combine(seed, std::hash<bool>{}(d.magFilter));
            hash_combine(seed, std::hash<bool>{}(d.mipFilter));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.u)));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.v)));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.w)));
            hash_combine(seed, std::hash<float>{}(d.border.r));
            hash_combine(seed, std::hash<float>{}(d.border.g));
            hash_combine(seed, std::hash<float>{}(d.border.b));
            hash_combine(seed, std::hash<float>{}(d.border.a));
            return seed;
        }
    };

    template<> struct hash<alina::TextureDesc> {
        size_t operator()(const alina::TextureDesc& d) const noexcept {
            size_t seed = 0;
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.format)));
            hash_combine(seed, std::hash<uint32_t>{}(d.width));
            hash_combine(seed, std::hash<uint32_t>{}(d.height));
            hash_combine(seed, std::hash<uint32_t>{}(d.depth));
            hash_combine(seed, std::hash<uint32_t>{}(d.mipLevels));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.usage)));
            hash_combine(seed, std::hash<std::string>{}(d.name));
            hash_combine(seed, std::hash<alina::SamplerDesc>{}(d.sampler));
            return seed;
        }
    };

    template<> struct hash<alina::VertexAttributeDesc> {
        size_t operator()(const alina::VertexAttributeDesc& d) const noexcept {
            size_t seed = 0;
            hash_combine(seed, std::hash<std::string>{}(d.name));
            hash_combine(seed, std::hash<uint32_t>{}(d.bufferIndex));
            hash_combine(seed, std::hash<uint32_t>{}(d.arraySize));
            hash_combine(seed, std::hash<uint32_t>{}(d.offset));
            hash_combine(seed, std::hash<uint32_t>{}(d.stride));
            hash_combine(seed, std::hash<bool>{}(d.isInstanced));
            hash_combine(seed, std::hash<int>{}(static_cast<int>(d.format)));
            return seed;
        }
    };
    template<>
    struct hash<std::vector<alina::VertexAttributeDesc>> {
        std::size_t operator()(const std::vector<alina::VertexAttributeDesc>& vec) const noexcept {
            std::size_t seed = 0;
            for (const auto& v : vec) {
                // Combine each element's hash
                seed ^= std::hash<alina::VertexAttributeDesc>{}(v) + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    template<> struct hash<alina::FramebufferDesc> {
        size_t operator()(const alina::FramebufferDesc& d) const noexcept {
            size_t seed = 0;
            for (const auto& att : d.colorAttachments) {
                hash_combine(seed, std::hash<void*>{}(att.texture.get()));
            }
            hash_combine(seed, std::hash<void*>{}(d.depthAttachment.texture.get()));
            hash_combine(seed, std::hash<std::string>{}(d.name));
            return seed;
        }
    };
}
