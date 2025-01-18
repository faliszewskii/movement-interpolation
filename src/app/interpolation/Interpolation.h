//
// Created by faliszewskii on 04.12.24.
//

#ifndef MOVEMENT_INTERPOLATION_INTERPOLATION_H
#define MOVEMENT_INTERPOLATION_INTERPOLATION_H


#include <glm/glm.hpp>
#include <vector>
#include "InterpolationType.h"
#include "Frame.h"

class Interpolation {
public:
    template<InterpolationType type> glm::mat4 interpolate(float t, Frame beginFrame, Frame endFrame);
    glm::mat4 interpolate(InterpolationType type, float t, Frame beginFrame, Frame endFrame);

    std::vector<glm::mat4> generateInBetweens(InterpolationType type, int count, Frame beginFrame, Frame endFrame);

    template<typename T> static T lerp(float t, T start, T end);
    static glm::quat slerp(float t, glm::quat a, glm::quat b);
private:
    glm::vec3 interpolateTranslation(float t, glm::vec3 begin, glm::vec3 end);

    glm::mat4 combineTransformation(glm::vec3 translation, glm::vec3 eulerRotation);
    glm::mat4 combineTransformation(glm::vec3 translation, glm::quat quatRotation);

};

template<typename T> T Interpolation::lerp(float t, T start, T end) {
    return start + (end - start) * t;
}


#endif //MOVEMENT_INTERPOLATION_INTERPOLATION_H
