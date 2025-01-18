//
// Created by faliszewskii on 16.06.24.
//

#include "Scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../interface/camera/CameraAnchor.h"

Scene::Scene(AppContext &appContext) : appContext(appContext) {
    appContext.camera = std::make_unique<CameraAnchor>(1920/2, 1080, glm::vec3(0.0f, 3.0f, 3.0f), glm::vec3(0.f), glm::vec3(-M_PI / 4, 0, 0));
    appContext.frameBufferManagerLeft = std::make_unique<FrameBufferManager>();
    appContext.frameBufferManagerLeft->create_buffers(appContext.camera->screenWidth, appContext.camera->screenHeight);
    appContext.frameBufferManagerRight = std::make_unique<FrameBufferManager>();
    appContext.frameBufferManagerRight->create_buffers(appContext.camera->screenWidth, appContext.camera->screenHeight);

    appContext.phongShader = std::make_unique<Shader>(Shader::createTraditionalShader(
            "../res/shaders/phong/phong.vert", "../res/shaders/phong/phong.frag"));
    appContext.pointShader = std::make_unique<Shader>(Shader::createTraditionalShader(
            "../res/shaders/point/point.vert", "../res/shaders/point/point.frag"));

    appContext.quad = std::make_unique<Quad>();
    appContext.light = std::make_unique<PointLight>();
    appContext.light->position = {-0.5f , 0.25f, 0.25f};


    appContext.axisR = std::make_unique<Cylinder>();
    appContext.axisG = std::make_unique<Cylinder>();
    appContext.axisB = std::make_unique<Cylinder>();

    appContext.grid = std::make_unique<Grid>();

    appContext.t = 0;
    appContext.leftTransformation = glm::identity<glm::mat4>();
    appContext.rightTransformation = glm::identity<glm::mat4>();

    appContext.interpolationRight = InterpolationType::QuaternionLinear;
    appContext.interpolationLeft = InterpolationType::Euler;

    appContext.running = false;
    appContext.simulationSpeed = 0.5;
    appContext.lastFrameTimeS = glfwGetTime();
    appContext.inBetweenCount = 0;

    // Puma

    appContext.robotL = std::make_unique<Robot>();
    appContext.robotR = std::make_unique<Robot>();

    appContext.beginFrame.translation.x = 2;
    appContext.beginFrame.translation.z = 2;
    appContext.endFrame.translation.x = 2;
    appContext.endFrame.translation.z = 2;
}

void Scene::update() {
    if(appContext.running) {
        float timeS = glfwGetTime();
        float dt = appContext.simulationSpeed * (timeS - appContext.lastFrameTimeS);
        appContext.t += dt;
        if(appContext.t > 1) {
            appContext.t = 1;
            appContext.running = false;
        }
        appContext.leftTransformation = appContext.interpolation.interpolate(appContext.interpolationLeft, appContext.t, appContext.beginFrame, appContext.endFrame);
        appContext.rightTransformation = appContext.interpolation.interpolate(appContext.interpolationRight, appContext.t, appContext.beginFrame, appContext.endFrame);
        appContext.lastFrameTimeS = timeS;
        appContext.robotR->interpolate(appContext.t, appContext.beginFrame, appContext.endFrame);
        appContext.robotL->interpolateParams(appContext.t, appContext.beginFrame, appContext.endFrame);
    } else {
        appContext.lastFrameTimeS = glfwGetTime();
    }
}

void Scene::render(FrameBufferManager &frameBufferManager, glm::mat4 transformation, std::vector<glm::mat4> &inBetweens, Robot& robot) {
    frameBufferManager.bind();

    appContext.phongShader->use();
    appContext.phongShader->setUniform("viewPos", appContext.camera->getViewPosition());
    appContext.phongShader->setUniform("view", appContext.camera->getViewMatrix());
    appContext.phongShader->setUniform("projection", appContext.camera->getProjectionMatrix());
    appContext.phongShader->setUniform("material.hasTexture", false);
    appContext.phongShader->setUniform("material.albedo", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    appContext.phongShader->setUniform("material.shininess", 256.f);
    appContext.light->setupPointLight(*appContext.phongShader);

    if(appContext.chosenMode == AppContext::RobotMode)
        robot.render(*appContext.phongShader);

    if(appContext.chosenMode == AppContext::AxisMode) {
        for(auto &m : inBetweens) {
            renderAxis(m, 0.2f);
        }
        renderAxis(transformation, 1.0f);
    } else {
        auto m = glm::identity<glm::mat4>();
        m = glm::translate(m, appContext.beginFrame.translation);
        m = m * glm::toMat4(appContext.beginFrame.quaternion);
        renderAxis(m, 0.2f);
        m = glm::identity<glm::mat4>();
        m = glm::translate(m, appContext.endFrame.translation);
        m = m * glm::toMat4(appContext.endFrame.quaternion);
        renderAxis(m, 0.2f);
    }

    glDisable(GL_CULL_FACE);
    appContext.grid->render(appContext);
    glEnable(GL_CULL_FACE);

    frameBufferManager.unbind();
}

void Scene::render() {
    render(*appContext.frameBufferManagerLeft, appContext.leftTransformation, appContext.inBetweenTransformationsLeft, *appContext.robotL);
    render(*appContext.frameBufferManagerRight, appContext.rightTransformation, appContext.inBetweenTransformationsRight, *appContext.robotR);
}

void Scene::renderAxis(glm::mat4 transformation, float alpha) {

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

    modelR = transformation * modelR;
    modelG = transformation * modelG;
    modelB = transformation * modelB;

    appContext.phongShader->setUniform("model", modelR);
    appContext.phongShader->setUniform("material.albedo", glm::vec4(1.0f, 0.0f, 0.0f, alpha));
    appContext.axisR->render();
    appContext.phongShader->setUniform("model", modelG);
    appContext.phongShader->setUniform("material.albedo", glm::vec4(0.0f, 1.0f, 0.0f, alpha));
    appContext.axisG->render();
    appContext.phongShader->setUniform("model", modelB);
    appContext.phongShader->setUniform("material.albedo", glm::vec4(0.0f, 0.0f, 1.0f, alpha));
    appContext.axisB->render();

}
