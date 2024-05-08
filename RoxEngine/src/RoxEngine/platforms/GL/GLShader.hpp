#pragma once
#include "RoxEngine/renderer/Shader.hpp"
#include <cstdint>
namespace RoxEngine::GL {
    class Shader final : public ::RoxEngine::Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~Shader() final;

        uint32_t mID;
    };
}