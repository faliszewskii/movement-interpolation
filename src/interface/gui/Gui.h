//
// Created by faliszewskii on 16.06.24.
//

#ifndef OPENGL_TEMPLATE_GUI_H
#define OPENGL_TEMPLATE_GUI_H


#include "../../app/AppContext.h"

class Gui {
    AppContext &appContext;

public:
    Gui(AppContext &appContext);

    void render();

    void InterpolationMenu();

    void InterpolationMenu(string side);

    glm::mat4 interpolate(InterpolationType type);

    void refreshInterpolation();
};


#endif //OPENGL_TEMPLATE_GUI_H
