#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct Transform
{
    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    Transform() = default;
    Transform(const Transform&) = default;
    Transform(const glm::vec3& translation)
        : translation(translation) {}

    glm::mat4 GetMatrix() const
    {
        return glm::translate(glm::mat4(1.0f), translation)
            * glm::toMat4(glm::quat(rotation))
            * glm::scale(glm::mat4(1.0f), scale);
    }
};