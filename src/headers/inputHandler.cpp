#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

#include "engine.hpp"
#include "inputHandler.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.141592653589793116 
#endif

namespace {
    // keyboard layout 
    enum { // короче, добавляешь новое объявление в enum, соответсвующую клавишу добавляешь в inputLayout
        MOVE_NORTH,
        MOVE_EAST,
        MOVE_SOUTH,
        MOVE_WEST,
        MOVE_UP,
        MOVE_DOWN
    };
    GLuint inputLayout[] = {
        GLFW_KEY_W,
        GLFW_KEY_D,
        GLFW_KEY_S,
        GLFW_KEY_A,
        GLFW_KEY_E,
        GLFW_KEY_Q
    };
    // keyboard layout end

    float _mouseSensitivity, _moveSpeed;

    GLFWwindow *_window;

    float fov;

    float _fov, _aspect, _near, _far; // _ значит что эта преременная не изменяется
    float _widthCenter, _heightCenter;

    
    float horAngle, vertAngle;
    glm::vec3 cameraPosition;

    float changeDistance, scale;

    int cubeScale = 1;
}
int getCameraCubeX() {
    glm::vec3 forward(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    return ((cameraPosition + forward * changeDistance).x) - float(cubeScale) / 2.0f;
}
int getCameraCubeY() {
    glm::vec3 forward(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    return ((cameraPosition + forward * changeDistance).y) - float(cubeScale) / 2.0f;
}
int getCameraCubeZ() {
    glm::vec3 forward(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    return ((cameraPosition + forward * changeDistance).z) - float(cubeScale) / 2.0f;
}

int getCameraCubeScale() {
    return cubeScale;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    cubeScale += yoffset;
    if (cubeScale < 1) {
        cubeScale = 1;
    }
    DEBUG_ONLY(printf("cubescale - %d\n", cubeScale));
    DEBUG_ONLY(fflush(stdout));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    glm::vec3 forward(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        DEBUG_ONLY(printf("add - %d %d %d %d\n",
                    int(((cameraPosition + forward * changeDistance).x - float(cubeScale) / 2.0f)),
                int(((cameraPosition + forward * changeDistance).y - float(cubeScale) / 2.0f)),
                int(((cameraPosition + forward * changeDistance).z - float(cubeScale) / 2.0f)), cubeScale));
        DEBUG_ONLY(fflush(stdout));
        update(((cameraPosition + forward * changeDistance).x - float(cubeScale) / 2.0f),
                ((cameraPosition + forward * changeDistance).y - float(cubeScale) / 2.0f),
                ((cameraPosition + forward * changeDistance).z - float(cubeScale) / 2.0f), cubeScale, true);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        DEBUG_ONLY(printf("rem - %d %d %d %d\n",
                    int(((cameraPosition + forward * changeDistance).x - float(cubeScale) / 2.0f)),
                int(((cameraPosition + forward * changeDistance).y - float(cubeScale) / 2.0f)),
                int(((cameraPosition + forward * changeDistance).z - float(cubeScale) / 2.0f)), cubeScale));
        DEBUG_ONLY(fflush(stdout));
        update(((cameraPosition + forward * changeDistance).x - float(cubeScale) / 2.0f),
                ((cameraPosition + forward * changeDistance).y - float(cubeScale) / 2.0f),
                ((cameraPosition + forward * changeDistance).z - float(cubeScale) / 2.0f), cubeScale, false);
    }
}

void inputHandlerInit(GLFWwindow *projWindow, float windowWidth, float windowHeight,
        float startFov, float near, float far,
        glm::vec3 inCameraPosition,
        float mouseSensitivity, float moveSpeed) {
    _window = projWindow;
    _aspect = (float) windowWidth / (float) windowHeight;
    _fov = startFov;
    fov = _fov;
    _near = near;
    _far = far;

    _widthCenter = windowWidth / 2;
    _heightCenter = windowHeight / 2;

    cameraPosition = inCameraPosition;

    _mouseSensitivity = mouseSensitivity;
    _moveSpeed = moveSpeed;

    horAngle = 0.0f;
    vertAngle = 0.0f;
    

    // set callbacks
    glfwSetScrollCallback(_window, scrollCallback);
    glfwSetMouseButtonCallback(_window, mouseButtonCallback);
    changeDistance = 10.0f;
    scale = 1;
}

void getVPMatrix(glm::mat4 &vp) {
#ifdef _DEBUG
    if (_window == NULL) {
        fprintf(stderr, "Input handler window error. Forgot to init?\n");
    }
#endif
    // get time delta
    static double lastTime = glfwGetTime();
    static double currentTime;
    currentTime = glfwGetTime();
    static float deltaTime;
    deltaTime = float(currentTime - lastTime);


    // get mouse delta
    static double xPos, yPos;
    glfwGetCursorPos(_window, &xPos, &yPos);
    glfwSetCursorPos(_window, _widthCenter, _heightCenter);
    horAngle += _mouseSensitivity * deltaTime * (_widthCenter - xPos);
    vertAngle += _mouseSensitivity * deltaTime * (_heightCenter - yPos);
    
    glm::vec3 forward = glm::vec3(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    glm::vec3 right = glm::vec3(
            sin(horAngle - M_PI/2.0f),
            0,
            cos(horAngle - M_PI/2.0f)
    );
    glm::vec3 up = glm::cross(right, forward);

    // get keyboard input
    // переделай в callbacks, мне кажется оно должно работать быстрее, чем каждый раз провеять))
    glm::vec3 cameraPositionChange = glm::vec3(_moveSpeed * deltaTime);
    if (glfwGetKey(_window, inputLayout[MOVE_NORTH]) == GLFW_PRESS) {
        cameraPosition += forward * cameraPositionChange;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_SOUTH]) == GLFW_PRESS) {
        cameraPosition -= forward * cameraPositionChange;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_EAST]) == GLFW_PRESS) {
        cameraPosition += right * cameraPositionChange;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_WEST]) == GLFW_PRESS) {
        cameraPosition -= right * cameraPositionChange;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_UP]) == GLFW_PRESS) {
        cameraPosition += up * cameraPositionChange;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_DOWN]) == GLFW_PRESS) {
        cameraPosition -= up * cameraPositionChange;
    }

    vp = glm::perspective(glm::radians(fov), _aspect, _near, _far) *
            glm::lookAt(cameraPosition, cameraPosition+forward, up) * glm::mat4(1.0f);

    // update last time (for delta time)
    lastTime = currentTime;
}
