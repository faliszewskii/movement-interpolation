//
// Created by USER on 18/01/2025.
//

#ifndef ROBOT_H
#define ROBOT_H
#include "../entity/cylinder/Cylinder.h"
#include "../interpolation/Frame.h"

class Robot {
public:
    Robot();

private:
    float angle1;
    float angle2;
    float angle3;
    float angle4;
    float angle5;

    float length1;
    float length2;
    float length3;
    float length4;

    Cylinder armModel;

public:
    void render(Shader shader);

    void solve(glm::mat4 frame);

    void interpolate(float t, Frame &beginFrame, Frame &endFrame);
    void interpolateParams(float t, Frame &beginFrame, Frame &endFrame);
};



#endif //ROBOT_H
