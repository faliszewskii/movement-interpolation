//
// Created by faliszewskii on 16.06.24.
//

#ifndef OPENGL_TEMPLATE_SCENE_H
#define OPENGL_TEMPLATE_SCENE_H

#include "AppContext.h"

class Scene {
    AppContext &appContext;

public:
    explicit Scene(AppContext &appContext);
    void update();
    void render();

    void render(FrameBufferManager &frameBufferManager, glm::mat4 transformation);
};


#endif //OPENGL_TEMPLATE_SCENE_H
