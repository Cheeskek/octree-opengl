#ifndef _INPUT_HANDLER
#define _INPUT_HANDLER

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void inputHandlerInit(GLFWwindow *projWindow, float windowWidth, float windowHeight, float startFov=100.0f, float near=0.1f, float far=300.0f, glm::vec3 inCameraPosition=glm::vec3(0,0,0), float mouseSensitivity=0.01f, float moveSpeed=30.0f);

void getVPMatrix(glm::mat4 &vp);

int getCameraCubeX();
int getCameraCubeY();
int getCameraCubeZ();
int getCameraCubeScale();

#endif
