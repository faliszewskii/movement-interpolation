//
// Created by faliszewskii on 04.12.24.
//

#ifndef MOVEMENT_INTERPOLATION_FRAME_H
#define MOVEMENT_INTERPOLATION_FRAME_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

struct Frame {
    glm::vec3 translation;
    glm::vec3 euler;
    glm::quat quaternion;

    Frame() {
        translation = glm::vec3();
        euler = glm::vec3();
        quaternion = glm::identity<glm::quat>();
    }
};

#endif //MOVEMENT_INTERPOLATION_FRAME_H
