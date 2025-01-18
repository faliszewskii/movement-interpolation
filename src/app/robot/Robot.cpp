//
// Created by USER on 18/01/2025.
//

#include "Robot.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../interpolation/Interpolation.h"

Robot::Robot() {
    length1 = 1;
    length2 = 1;
    length3 = 1;
    length4 = 1;
    angle1 = 0;
    angle2 = 0;
    angle3 = 0;
    angle4 = 0;
    angle5 = 0;
}

void Robot::render(Shader shader) {
    auto I = glm::identity<glm::mat4>();
    auto model = I;

    auto F1 = glm::rotate(I, angle1, glm::vec3(0, 0, 1));

    auto F1Prim = F1 * glm::translate(I, glm::vec3(0, 0, length1));
    auto F2 = F1Prim * glm::rotate(I, angle2, glm::vec3(0, 1, 0));

    auto F2Prim = F2 * glm::translate(I, glm::vec3(length2, 0, 0));
    auto F3 = F2Prim * glm::rotate(I, angle3, glm::vec3(0, 1, 0));

    auto F3Prim = F3 * glm::translate(I, glm::vec3(0, 0, length3));
    auto F4 = F3Prim * glm::rotate(I, angle4, glm::vec3(0, 0, 1));

    auto F4Prim = F4 * glm::translate(I, glm::vec3(length4, 0, 0));
    auto F5 = F4Prim * glm::rotate(I, angle5, glm::vec3(1, 0, 0));

    model = F1;
    model = glm::rotate(model, glm::half_pi<float>(), { 1.0f, 0.0f, 0.0f });
    model = glm::scale(model, glm::vec3(1, length1, 1));
    model = glm::translate(model, glm::vec3(0, 0.5, 0));


    shader.setUniform("material.albedo", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    shader.setUniform("model", model);
    armModel.render();

    model = F2;
    model = glm::rotate(model, -glm::half_pi<float>(), { 0.0f, 0.0f, 1.0f });
    model = glm::scale(model, glm::vec3(1, length2, 1));
    model = glm::translate(model, glm::vec3(0, 0.5, 0));

    shader.setUniform("material.albedo", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    shader.setUniform("model", model);
    armModel.render();

    model = F3;
    model = glm::rotate(model, glm::half_pi<float>(), { 1.0f, 0.0f, 0.0f });
    model = glm::scale(model, glm::vec3(1, length3, 1));
    model = glm::translate(model, glm::vec3(0, 0.5, 0));

    shader.setUniform("material.albedo", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    shader.setUniform("model", model);
    armModel.render();

    model = F4;
    model = glm::rotate(model, -glm::half_pi<float>(), { 0.0f, 0.0f, 1.0f });
    model = glm::scale(model, glm::vec3(1, length4, 1));
    model = glm::translate(model, glm::vec3(0, 0.5, 0));

    shader.setUniform("material.albedo", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    shader.setUniform("model", model);
    armModel.render();

    auto modelR = glm::identity<glm::mat4>();
    modelR = glm::rotate(modelR, float(-std::numbers::pi/2), glm::vec3(0, 0, 1));
    modelR = glm::scale(modelR, glm::vec3(.2, 1, .2));
    modelR = glm::translate(modelR, glm::vec3(0, 0.5, 0));

    auto modelG = glm::identity<glm::mat4>();
    modelG = glm::scale(modelG, glm::vec3(.2, 1, .2));
    modelG = glm::translate(modelG, glm::vec3(0, 0.5, 0));

    auto modelB = glm::identity<glm::mat4>();
    modelB = glm::rotate(modelB, float(std::numbers::pi/2), glm::vec3(1, 0, 0));
    modelB = glm::scale(modelB, glm::vec3(.2, 1, .2));
    modelB = glm::translate(modelB, glm::vec3(0, 0.5, 0));

    modelR = F5 * modelR;
    modelG = F5 * modelG;
    modelB = F5 * modelB;

    shader.setUniform("model", modelR);
    shader.setUniform("material.albedo", glm::vec4(1.0f, 0.0f, 0.0f, 1));
    armModel.render();
    shader.setUniform("model", modelG);
    shader.setUniform("material.albedo", glm::vec4(0.0f, 1.0f, 0.0f, 1));
    armModel.render();
    shader.setUniform("model", modelB);
    shader.setUniform("material.albedo", glm::vec4(0.0f, 0.0f, 1.0f, 1));
    armModel.render();

}


static glm::vec2 projectToPlane(const glm::vec3& point, const glm::vec3& u, const glm::vec3& v)
{
    return glm::vec2(glm::dot(point, u), glm::dot(point, v));
};
static float angleAroundAxis(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& u, const glm::vec3& v)
{
    glm::vec2 u1 = glm::normalize(projectToPlane(v1, u, v));
    glm::vec2 u2 = glm::normalize(projectToPlane(v2, u, v));
    return atan2f(u1.x * u2.y - u1.y * u2.x, u1.x * u2.x + u1.y * u2.y);
};

void Robot::solve(glm::mat4 frame) {
    float angle1Old = angle1;

    auto Fprime0 = glm::mat4(1.0f);
    auto F0 = glm::rotate(Fprime0, angle1, glm::vec3(0.0f, 0.0f, 1.0f));


    auto Fprime1 = glm::translate(F0, glm::vec3(0.0f, 0.0f, length1));
    auto F1 = glm::rotate(Fprime1, angle2, glm::vec3(0.0f, 1.0f, 0.0f));

    auto Fprime2 = glm::translate(F1, glm::vec3(length2, 0.0f, 0.0f));
    auto F2 = glm::rotate(Fprime2, angle3, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 matrix = frame;
    glm::vec3 X5 = matrix[0];
    glm::vec3 Y5 = matrix[1];
    glm::vec3 Z5 = matrix[2];
    glm::vec3 P5 = matrix[3];
    glm::vec3 P1 = { 0.0f, 0.0f, 0.0f };
    glm::vec3 P4 = P5 - X5 * length4;
    angle1 = atan2f(P4.y, P4.x);

    if(abs(angle1 - angle1Old) > abs(angle1 + std::numbers::pi_v<float> - angle1Old))
    {
        angle1 += std::numbers::pi_v<float>;
    }

    glm::vec3 X1 = glm::rotate(glm::mat4(1.0f), angle1, glm::vec3(0.0f, 0.0f, 1.0f))[0];
    glm::vec3 Z1 = { 0.0f, 0.0f, 1.0f };
    glm::vec3 Y1 = glm::cross(Z1, X1);
    glm::vec3 P2 = P1 + Z1 * length1;

    glm::vec3 P3P4direction = glm::normalize(glm::cross(Y1, X5));

    glm::vec3 P31 = P4 + P3P4direction * length3;
    glm::vec3 P32 = P4 - P3P4direction * length3;
    glm::vec3 P3ref = F2[3];

    glm::vec3 P3 = P31;
    if(glm::distance(P31, P3ref) < glm::distance(P32, P3ref))
    {
        P3 = P31;
    }
    else
    {
        P3 = P32;
    }
    length2 = glm::distance(P2, P3);

    glm::vec3 X2 = glm::normalize(P3 - P2);
    glm::vec3 Y2 = Y1;
    glm::vec3 Z2 = glm::normalize(glm::cross(X2, Y2));
    angle2 = angleAroundAxis(X2, X1, X1, Z1);

    glm::vec3 Z3 = glm::normalize(P4 - P3);
    glm::vec3 Y3 = Y2;
    glm::vec3 X3 = glm::normalize(glm::cross(Y3, Z3));
    angle3 = angleAroundAxis(Z3, Z2, X2, Z2);


    glm::vec3 X4 = X5;
    glm::vec3 Z4 = Z3;
    glm::vec3 Y4 = glm::normalize(glm::cross(Z4, X4));
    angle4 = angleAroundAxis(X4, X3, Y3, X3);

    angle5 = angleAroundAxis(Z5, Z4, Z4, Y4);
}

void Robot::interpolate(float t, Frame &beginFrame, Frame &endFrame) {
    glm::vec3 translation = Interpolation::lerp(t, beginFrame.translation, endFrame.translation);
    glm::quat orientation = Interpolation::slerp(t, beginFrame.quaternion, endFrame.quaternion);
    auto m = glm::identity<glm::mat4>();
    m = glm::translate(m, translation);
    m = m * glm::toMat4(orientation);
    solve(m);
}

void Robot::interpolateParams(float t, Frame &beginFrame, Frame &endFrame) {
    auto m = glm::identity<glm::mat4>();
    m = glm::translate(m, beginFrame.translation);
    m = m * glm::toMat4(beginFrame.quaternion);
    solve(m);
    float alpha1 = angle1;
    float alpha2 = angle2;
    float alpha3 = angle3;
    float alpha4 = angle4;
    float alpha5 = angle5;
    float start = length2;
    m = glm::identity<glm::mat4>();
    m = glm::translate(m, endFrame.translation);
    m = m * glm::toMat4(endFrame.quaternion);
    solve(m);
    float beta1 = angle1;
    float beta2 = angle2;
    float beta3 = angle3;
    float beta4 = angle4;
    float beta5 = angle5;
    float end = length2;

    angle1 = Interpolation::lerp(t, alpha1, beta1);
    angle2 = Interpolation::lerp(t, alpha2, beta2);
    angle3 = Interpolation::lerp(t, alpha3, beta3);
    angle4 = Interpolation::lerp(t, alpha4, beta4);
    angle5 = Interpolation::lerp(t, alpha5, beta5);
    length2 = Interpolation::lerp(t, start, end);

    while(angle1 < -std::numbers::pi) angle1 += 2 * std::numbers::pi;
    while(angle1 >= std::numbers::pi) angle1 -= 2 * std::numbers::pi;
    while(angle2 < -std::numbers::pi) angle2 += 2 * std::numbers::pi;
    while(angle2 >= std::numbers::pi) angle2 -= 2 * std::numbers::pi;
    while(angle3 < -std::numbers::pi) angle3 += 2 * std::numbers::pi;
    while(angle3 >= std::numbers::pi) angle3 -= 2 * std::numbers::pi;
    while(angle4 < -std::numbers::pi) angle4 += 2 * std::numbers::pi;
    while(angle4 >= std::numbers::pi) angle4 -= 2 * std::numbers::pi;
    while(angle5 < -std::numbers::pi) angle5 += 2 * std::numbers::pi;
    while(angle5 >= std::numbers::pi) angle5 -= 2 * std::numbers::pi;
}

