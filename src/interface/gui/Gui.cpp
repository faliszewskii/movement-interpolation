//
// Created by faliszewskii on 16.06.24.
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Gui.h"
#include "imgui.h"
#include <glm/gtx/string_cast.hpp>

Gui::Gui(AppContext &appContext) : appContext(appContext) {}


glm::vec3 ToEulerAngles(glm::quat q) {
    glm::vec3 angles;
    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.x = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = std::sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
    double cosp = std::sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
    angles.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.z = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}

void Gui::render() {
    ImGui::Begin("Interpolation");

    const char* items[] = {
            "Euler angles",
            "Quaternion linear",
            "Quaternion spherical"
    };

    bool modified = false;
    bool eulerBeginModified = false;
    bool quatBeginModified = false;
    bool eulerEndModified = false;
    bool quatEndModified = false;

    ImGui::SeparatorText("Translation");
    ImGui::PushItemWidth(80);
    ImGui::BeginGroup();
    ImGui::Text("Begin");
    modified |= ImGui::DragFloat("x##LT", &appContext.beginFrame.translation.x, 0.01);
    modified |= ImGui::DragFloat("y##LT", &appContext.beginFrame.translation.y, 0.01);
    modified |= ImGui::DragFloat("z##LT", &appContext.beginFrame.translation.z, 0.01);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Text("End");
    modified |= ImGui::DragFloat("x##RT", &appContext.endFrame.translation.x, 0.01);
    modified |= ImGui::DragFloat("y##RT", &appContext.endFrame.translation.y, 0.01);
    modified |= ImGui::DragFloat("z##RT", &appContext.endFrame.translation.z, 0.01);
    ImGui::EndGroup();
    ImGui::PopItemWidth();

    ImGui::SeparatorText("Euler angles");
    ImGui::PushItemWidth(80);
    ImGui::BeginGroup();
    ImGui::Text("Begin");
    eulerBeginModified |= ImGui::DragFloat("x##LE", &appContext.beginFrame.euler.x, 0.1);
    eulerBeginModified |= ImGui::DragFloat("y##LE", &appContext.beginFrame.euler.y, 0.1);
    eulerBeginModified |= ImGui::DragFloat("z##LE", &appContext.beginFrame.euler.z, 0.1);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Text("End");
    eulerEndModified |= ImGui::DragFloat("x##RE", &appContext.endFrame.euler.x, 0.1);
    eulerEndModified |= ImGui::DragFloat("y##RE", &appContext.endFrame.euler.y, 0.1);
    eulerEndModified |= ImGui::DragFloat("z##RE", &appContext.endFrame.euler.z, 0.1);
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    modified |= eulerEndModified | eulerBeginModified;

    ImGui::SeparatorText("Quaternion rotation");
    ImGui::PushItemWidth(80);
    ImGui::BeginGroup();
    ImGui::Text("Begin");
    quatBeginModified |= ImGui::DragFloat("x##LQ", &appContext.beginFrame.quaternion.x, 0.01);
    quatBeginModified |= ImGui::DragFloat("y##LQ", &appContext.beginFrame.quaternion.y, 0.01);
    quatBeginModified |= ImGui::DragFloat("z##LQ", &appContext.beginFrame.quaternion.z, 0.01);
    quatBeginModified |= ImGui::DragFloat("w##LQ", &appContext.beginFrame.quaternion.w, 0.01);
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Text("End");
    quatEndModified |= ImGui::DragFloat("x##RQ", &appContext.endFrame.quaternion.x, 0.01);
    quatEndModified |= ImGui::DragFloat("y##RQ", &appContext.endFrame.quaternion.y, 0.01);
    quatEndModified |= ImGui::DragFloat("z##RQ", &appContext.endFrame.quaternion.z, 0.01);
    quatEndModified |= ImGui::DragFloat("w##RQ", &appContext.endFrame.quaternion.w, 0.01);
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    modified |= quatEndModified | quatBeginModified;

    if(eulerBeginModified) {
        auto z = angleAxis(glm::radians(appContext.beginFrame.euler.z), glm::vec3(0,0,1));
        auto y = angleAxis(glm::radians(appContext.beginFrame.euler.y), glm::vec3(0,1,0));
        auto x = angleAxis(glm::radians(appContext.beginFrame.euler.x), glm::vec3(1,0,0));
        appContext.beginFrame.quaternion = glm::normalize(z * y * x);
    }
    if(eulerEndModified) {
        auto z = angleAxis(glm::radians(appContext.endFrame.euler.z), glm::vec3(0,0,1));
        auto y = angleAxis(glm::radians(appContext.endFrame.euler.y), glm::vec3(0,1,0));
        auto x = angleAxis(glm::radians(appContext.endFrame.euler.x), glm::vec3(1,0,0));
        appContext.endFrame.quaternion = glm::normalize(z * y * x);
    }
    if(quatBeginModified) {
        appContext.beginFrame.quaternion = glm::normalize(appContext.beginFrame.quaternion);
        glm::vec3 euler = glm::eulerAngles(appContext.beginFrame.quaternion);
        appContext.beginFrame.euler = degrees(euler);
    }
    if(quatEndModified) {
        appContext.endFrame.quaternion = glm::normalize(appContext.endFrame.quaternion);
        glm::vec3 euler = glm::eulerAngles(appContext.endFrame.quaternion);
        appContext.endFrame.euler = glm::degrees(euler);
    }

    ImGui::SeparatorText("Interpolations");
    ImGui::Text("Interpolation Left");
    modified |= ImGui::Combo("##Interpolation Left", reinterpret_cast<int*>(&appContext.interpolationLeft), items, 3);
    ImGui::Text("Interpolation Right");
    modified |= ImGui::Combo("##Interpolation Right", reinterpret_cast<int*>(&appContext.interpolationRight), items, 3);

    ImGui::SeparatorText("Simulation");
    modified |= ImGui::SliderFloat("t", &appContext.t, 0, 1);
    ImGui::DragFloat("Speed", &appContext.simulationSpeed, 0.01, 0.1, 5);
    modified |= ImGui::InputInt("In-betweens", &appContext.inBetweenCount, 1);
    if(ImGui::Button(appContext.running? "Stop": "Start")) {
        if(!appContext.running && appContext.t == 1) appContext.t = 0;
        appContext.running = !appContext.running;
    }

    ImGui::End();

    if(modified)
        refreshInterpolation();
}

void Gui::refreshInterpolation() {
    appContext.leftTransformation = appContext.interpolation.interpolate(appContext.interpolationLeft, appContext.t, appContext.beginFrame, appContext.endFrame);
    appContext.rightTransformation = appContext.interpolation.interpolate(appContext.interpolationRight, appContext.t, appContext.beginFrame, appContext.endFrame);
    appContext.inBetweenTransformationsLeft = appContext.interpolation.generateInBetweens(appContext.interpolationLeft, appContext.inBetweenCount, appContext.beginFrame, appContext.endFrame);
    appContext.inBetweenTransformationsRight = appContext.interpolation.generateInBetweens(appContext.interpolationRight, appContext.inBetweenCount, appContext.beginFrame, appContext.endFrame);
}
