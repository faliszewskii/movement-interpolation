//
// Created by faliszewskii on 04.12.24.
//
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/detail/type_quat.hpp>
#include "Interpolation.h"
#include <glm/gtx/quaternion.hpp>
#include <numbers>
#include <algorithm>
#include "glm/gtx/euler_angles.hpp"

glm::quat Interpolation::slerp(float t, glm::quat a, glm::quat b)
{
    // Adapted from javax.vecmath.Quat4f
    float ratio = t;

    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    if (dot < 0)
    {
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
        dot = -dot;
    }
    float epsilon = 1e-6f;
    float s0, s1;
    if ((1.0 - dot) > epsilon)
    {
        float omega = (float)acos(dot);
        float invSinOmega = 1.0f / (float)sin(omega);
        s0 = (float)sin((1.0 - ratio) * omega) * invSinOmega;
        s1 = (float)sin(ratio * omega) * invSinOmega;
    }
    else
    {
        s0 = 1.0f - ratio;
        s1 = ratio;
    }
    float rx = s0 * a.x + s1 * b.x;
    float ry = s0 * a.y + s1 * b.y;
    float rz = s0 * a.z + s1 * b.z;
    float rw = s0 * a.w + s1 * b.w;
    return glm::quat(rw, rx, ry, rz);
}// Utility function to wrap an angle to the range (-pi, pi)
double wrapAngle(double angle) {
    while (angle <= -M_PI) angle += 2 * M_PI;
    while (angle > M_PI) angle -= 2 * M_PI;
    return angle;
}

// Linear interpolation between two angles in the range (-pi, pi)
double Interpolation::interpolateAngle(double angle1, double angle2, double t) {
    // Ensure angles are in the range (-pi, pi)
    angle1 = wrapAngle(angle1);
    angle2 = wrapAngle(angle2);

    // Compute the difference, accounting for cyclic nature
    double delta = angle2 - angle1;
    if (delta > M_PI) {
        delta -= 2 * M_PI;
    } else if (delta < -M_PI) {
        delta += 2 * M_PI;
    }

    // Perform the interpolation
    double result = angle1 + t * delta;

    // Wrap the result to (-pi, pi)
    return wrapAngle(result);
}

template<>
glm::mat4 Interpolation::interpolate<InterpolationType::Euler>(float t, Frame beginFrame, Frame endFrame) {
    glm::vec3 translation = interpolateTranslation(t, beginFrame.translation, endFrame.translation);
    glm::vec3 rotation;
    for(int i = 0; i < 3; i++) {
        rotation[i] = interpolateAngle(beginFrame.euler[i], endFrame.euler[i], t);
    }
    return combineTransformation(translation, rotation);
}
template<>
glm::mat4 Interpolation::interpolate<InterpolationType::QuaternionLinear>(float t, Frame beginFrame, Frame endFrame) {
    glm::vec3 translation = interpolateTranslation(t, beginFrame.translation, endFrame.translation);

    glm::quat adjustedEnd = endFrame.quaternion;
    if (glm::dot(beginFrame.quaternion, endFrame.quaternion) < 0.0) {
        adjustedEnd = -adjustedEnd;
    }

    glm::quat rotation = beginFrame.quaternion + (adjustedEnd - beginFrame.quaternion) * t;
    rotation = glm::normalize(rotation);
    return combineTransformation(translation, rotation);
}
template<>
glm::mat4 Interpolation::interpolate<InterpolationType::QuaternionSpherical>(float t, Frame beginFrame, Frame endFrame) {
    glm::vec3 translation = interpolateTranslation(t, beginFrame.translation, endFrame.translation);
    return combineTransformation(translation, slerp(t, beginFrame.quaternion, endFrame.quaternion));
}

glm::vec3 Interpolation::interpolateTranslation(float t, glm::vec3 begin, glm::vec3 end) {
    return begin + (end - begin) * t;
}

glm::mat4 Interpolation::combineTransformation(glm::vec3 translation, glm::vec3 eulerRotation) {
    eulerRotation = glm::radians(eulerRotation);
    auto m = glm::identity<glm::mat4>();
    m = glm::translate(m, translation);
    auto quaternion = glm::quat(eulerRotation);
    glm::mat4 quaternionMatrix = glm::toMat4(quaternion);
    return m * quaternionMatrix;
}

glm::mat4 Interpolation::combineTransformation(glm::vec3 translation, glm::quat quatRotation) {
    auto m = glm::identity<glm::mat4>();
    m = glm::translate(m, translation);
    m = m * glm::toMat4(quatRotation);
    return m;
}

glm::mat4 Interpolation::interpolate(InterpolationType type, float t, Frame beginFrame, Frame endFrame) {
    switch (type) {
        case Euler:
            return interpolate<Euler>(t, beginFrame, endFrame);
        case QuaternionLinear:
            return interpolate<QuaternionLinear>(t, beginFrame, endFrame);
        case QuaternionSpherical:
            return interpolate<QuaternionSpherical>(t, beginFrame, endFrame);
    }
}

std::vector<glm::mat4>
Interpolation::generateInBetweens(InterpolationType type, int count, Frame beginFrame, Frame endFrame) {
    std::vector<glm::mat4> inBetweens;
    if(count == 0) return inBetweens;
    std::vector<float> ts(count - 1);
    int c = 0;
    std::ranges::generate(ts, [c, count] mutable { return (float(c++ + 1) / float(count - 1)); });
    ts.insert(ts.begin(), 0);

    for(auto t : ts) {
        inBetweens.push_back(interpolate(type, t, beginFrame, endFrame));
    }
    return inBetweens;
}


