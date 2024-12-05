//
// Created by faliszewskii on 16.06.24.
//

#ifndef OPENGL_TEMPLATE_APPCONTEXT_H
#define OPENGL_TEMPLATE_APPCONTEXT_H

#include <memory>
#include "../opengl/framebuffer/FrameBufferManager.h"
#include "../interface/camera/BaseCamera.h"
#include "entity/quad/Quad.h"
#include "../opengl/shader/Shader.h"
#include "entity/light/PointLight.h"
#include "entity/point/Point.h"
#include "../opengl/model/Model.h"
#include "entity/cylinder/Cylinder.h"
#include "entity/grid/Grid.h"
#include "interpolation/InterpolationType.h"
#include "interpolation/Interpolation.h"
#include "interpolation/Frame.h"

struct AppContext {
    AppContext() = default;

    std::unique_ptr<BaseCamera> camera;
    std::unique_ptr<FrameBufferManager> frameBufferManagerLeft;
    std::unique_ptr<FrameBufferManager> frameBufferManagerRight;

    // Shaders
    std::unique_ptr<Shader> phongShader;
    std::unique_ptr<Shader> pointShader;

    // TODO --- App data goes here
    std::unique_ptr<PointLight> light;
    std::unique_ptr<Quad> quad;
    std::unique_ptr<Model> bunny;

    std::unique_ptr<Cylinder> axisR;
    std::unique_ptr<Cylinder> axisG;
    std::unique_ptr<Cylinder> axisB;

    std::unique_ptr<Grid> grid;

    Interpolation interpolation;

    InterpolationType interpolationLeft;
    InterpolationType interpolationRight;

    Frame beginFrame;
    Frame endFrame;

    glm::mat4 leftTransformation;
    glm::mat4 rightTransformation;

    std::vector<glm::mat4> inBetweenTransformationsLeft;
    std::vector<glm::mat4> inBetweenTransformationsRight;
    int inBetweenCount;

    float t;
    float simulationSpeed;
    bool running;
    float lastFrameTimeS;
};

#endif //OPENGL_TEMPLATE_APPCONTEXT_H
