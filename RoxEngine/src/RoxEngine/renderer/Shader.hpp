#pragma once
#include <string>
#include "RoxEngine/utils/Memory.h"

namespace RoxEngine {
    class Shader {
		public:
      static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
    };
}