#pragma once
#include <unordered_map>
#include <alina/alina.hpp>
#include <RoxEngine/renderer/alina/Utils.hpp>
namespace RoxEngine::AlinaGlue {
    template<typename Desc, typename Tp, typename Creator
    > requires (
        // Desc must be hashable
        requires(const Desc& d) {
            { std::hash<Desc>{}(d) } -> std::convertible_to<std::size_t>;
        } &&
        requires(Creator c, const Desc& d) {
            { c.Create(d) } -> std::convertible_to<Tp>;
        }
    )
    class ResourcePool {
    public:
        template<typename... Args>
        ResourcePool(Args&&... args) : mCreator(std::forward<Args>(args)...) {}
        Tp& Get(const Desc& desc) {
            auto it = mData.find(desc);
            if(it != mData.end())
                return it->second;
            Tp resource;
            resource = mCreator.Create(desc);
            auto [insertedIt, success] = mData.emplace(desc, std::move(resource));
            return insertedIt->second;
        }
    private:
        std::unordered_map<Desc, Tp> mData;
        Creator mCreator;
    };
}
namespace RoxEngine::AlinaGlue {
    class AlinaResourceCreator {
    public:
        AlinaResourceCreator(alina::Device device) {mDevice = device;}
        alina::InputLayout Create(const std::vector<alina::VertexAttributeDesc>& attrs) {
            return mDevice->createInputLayout(attrs);
        }
        alina::GraphicsPipeline Create(const alina::GraphicsPipelineDesc& desc) {
            return mDevice->createGraphicsPipeline(desc);
        }
    private:
        alina::Device mDevice;
    };

    using InputLayoutPool = ResourcePool<std::vector<alina::VertexAttributeDesc>, alina::InputLayout, AlinaResourceCreator>;
    using GraphicsPipelinePool = ResourcePool<alina::GraphicsPipelineDesc, alina::GraphicsPipeline, AlinaResourceCreator>;
}