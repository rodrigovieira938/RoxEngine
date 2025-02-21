#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace RoxEngine {
    class Transform {
    public:
        Transform() = default;
        inline Transform(glm::vec3 position, glm::vec3 rotation = {0,0,0}, glm::vec3 scale = {1,1,1}) {
            this->position = position;
            this->rotation = rotation;
            this->scale = scale;
        }
        inline glm::mat4 RecalculateMatrix() {
            return glm::translate(glm::mat4(1.0f), this->position) * glm::scale(glm::mat4(1.0f), scale) * glm::toMat4(glm::quat(this->rotation));
        }

        glm::vec3 position = {0,0,0};
        glm::vec3 rotation = {0,0,0};
        glm::vec3 scale = {1,1,1};
    };
}