cmake_minimum_required(VERSION 3.28)
project(movement-interpolation)

set(CMAKE_CXX_STANDARD 26)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always -Wall -g")

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")
endif()

# Dear ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/dep/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)
set(IMGUI_SRC
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_subdirectory(dep/glfw)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/dep/inc)

# Link directories
link_directories(${CMAKE_SOURCE_DIR}/dep/lib)

set(APP_SRC
        src/interface/gui/GuiBase.cpp
        src/interface/gui/GuiBase.h
        src/app/AppContext.h
        src/interface/gui/Gui.cpp
        src/interface/gui/Gui.h
        src/opengl/framebuffer/FrameBufferManager.cpp
        src/opengl/framebuffer/FrameBufferManager.h
        src/opengl/shader/Shader.h
        src/opengl/shader/Shader.cpp
        src/opengl/mesh/is_vertex.h
        src/opengl/mesh/PositionNormalVertex.h
        src/opengl/mesh/PositionVertex.h
        src/opengl/mesh/PosNorTexVertex.h
        src/opengl/mesh/Mesh.h
        src/interface/camera/BaseCamera.h
        src/interface/camera/CameraAnchor.cpp
        src/interface/camera/CameraAnchor.h
        src/interface/input/InputHandler.cpp
        src/interface/input/InputHandler.h
        src/app/Scene.cpp
        src/app/Scene.h
        src/app/entity/light/PointLight.h
        src/app/entity/point/Point.h
        src/app/entity/cylinder/Cylinder.h
        src/app/entity/cylinder/Cylinder.cpp
        src/app/entity/grid/Grid.h
        src/app/entity/grid/Grid.cpp
        src/app/interpolation/InterpolationType.h
        src/app/interpolation/Interpolation.cpp
        src/app/interpolation/Interpolation.h
        src/app/interpolation/Frame.h
        src/app/robot/Robot.cpp
        src/app/robot/Robot.h)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(GLAD_GLEW
        dep/glad/glad.c
        dep/glad/glad_glfw.h
    )
else()
    set(GLAD_GLEW
        dep/glew/glad.c
        dep/glew/glew_glfw.h
    )

endif()

# Add executables
add_executable(movement-interpolation
        # Dependencies
        ${IMGUI_SRC}
        # Application
        ${CMAKE_SOURCE_DIR}/src/main.cpp
        ${GLAD_GLEW}
        ${APP_SRC}
)


# Link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")             # macOS
    target_link_libraries(movement-interpolation
            ${CMAKE_SOURCE_DIR}/dep/lib/libglfw.3.4.dylib
            ${CMAKE_SOURCE_DIR}/dep/lib/libGLEW.2.2.0.dylib
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework CoreFoundation"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")        # Windows
    find_package(glm CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    include_directories( ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(movement-interpolation PRIVATE
            glm::glm
            glfw
            ${OPENGL_LIBRARIES}
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")          # Linux
    target_link_libraries(movement-interpolation
            -lglfw
            -lGLEW
            -lGL
            -lX11
            -lpthread
            -lXrandr
            -lXi
            -ldl
    )
endif()