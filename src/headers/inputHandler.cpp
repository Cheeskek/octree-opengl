#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <stdio.h>

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
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
        GLFW_KEY_Q,
        GLFW_KEY_E
    };
    // keyboard layout end

    float _mouseSensitivity, _moveSpeed;

    GLFWwindow *_window;

    float fov;

    float _fov, _aspect, _near, _far; // _ значит что эта преременная не изменяется
    float _widthCenter, _heightCenter;

    
    float horAngle, vertAngle;
    glm::vec3 cameraPosition;
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= 5 * yoffset;
}


void inputHandlerInit(GLFWwindow *projWindow, float windowWidth, float windowHeight,
        float startFov = 60.0f, float near = 0.1f, float far = 100.0f,
        glm::vec3 inCameraPosition = glm::vec3(0,0,0),
        float mouseSensitivity = 0.01f, float moveSpeed = 10.0f) {
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
}

void getMVPMatrix(glm::mat4 &mvp) {
#ifdef _DEBUG
    if (_window == NULL) {
        fprintf(stderr, "Input handler window error. Forgot to init?\n");
    }
#endif
    // get time delta
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);


    // get mouse delta
    static double xPos, yPos;
    glfwGetCursorPos(_window, &xPos, &yPos);
    glfwSetCursorPos(_window, _widthCenter, _heightCenter);
    horAngle += _mouseSensitivity * deltaTime * (_widthCenter - xPos);
    vertAngle += _mouseSensitivity * deltaTime * (_heightCenter - yPos);
    
    glm::vec3 forward(
            cos(vertAngle) * sin(horAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horAngle)
    );
    glm::vec3 right(
            sin(horAngle - M_PI/2.0f),
            0,
            cos(horAngle - M_PI/2.0f)
    );
    glm::vec3 up = glm::cross(right, forward);

    // get keyboard input
    // переделай в callbacks, мне кажется оно должно работать быстрее, чем каждый раз провеять))
    if (glfwGetKey(_window, inputLayout[MOVE_NORTH]) == GLFW_PRESS) {
        cameraPosition += forward * deltaTime * _moveSpeed;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_SOUTH]) == GLFW_PRESS) {
        cameraPosition -= forward * deltaTime * _moveSpeed;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_EAST]) == GLFW_PRESS) {
        cameraPosition += right * deltaTime * _moveSpeed;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_WEST]) == GLFW_PRESS) {
        cameraPosition -= right * deltaTime * _moveSpeed;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_UP]) == GLFW_PRESS) {
        cameraPosition += up * deltaTime * _moveSpeed;
    }
    if (glfwGetKey(_window, inputLayout[MOVE_DOWN]) == GLFW_PRESS) {
        cameraPosition -= up * deltaTime * _moveSpeed;
    }

    mvp = glm::perspective(glm::radians(fov), _aspect, _near, _far) *
            glm::lookAt(cameraPosition, cameraPosition+forward, up) * glm::mat4(1.0f);

    // update last time (for delta time)
    lastTime = currentTime;
}
