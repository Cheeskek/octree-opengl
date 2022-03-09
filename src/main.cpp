#include "headers/shader.hpp"
#include "headers/inputHandler.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

int main() {
    // glew init
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
    }

    // window hints
    glfwWindowHint(GLFW_SAMPLES, 3);               // 2x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy;
    glfwWindowHint(GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE); // no old opengl

    DEBUG_ONLY(glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE));

    // Open a window and create its OpenGL context
    GLFWwindow *window;
    unsigned WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl_window-wow",
            NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // for glfw to see keys pressed
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // setup vertex array object
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // cube vertex data
    static const int gVertexBufferData[] = {
        1,  1,  1,
        1,  1, -1,
        -1,  1,  1,
        -1,  1,  1,
        1,  1, -1,
        -1,  1, -1,

        1, -1,  1,
        -1, -1,  1,
        1, -1, -1,
        1, -1, -1,
        -1, -1,  1,
        -1, -1, -1,

        1,  1,  1,
        1, -1,  1,
        1,  1, -1,
        1,  1, -1,
        1, -1,  1,
        1, -1, -1,

        -1,  1,  1,
        -1,  1, -1,
        -1, -1,  1,
        -1, -1,  1,
        -1,  1, -1,
        -1, -1, -1,

        1,  1,  1,
        -1,  1,  1,
        1, -1,  1,
        1, -1,  1,
        -1,  1,  1,
        -1, -1,  1,

        1,  1, -1,
        1, -1, -1,
        -1,  1, -1,
        -1,  1, -1,
        1, -1, -1,
        -1, -1, -1
    };

    // vertex normal data
    static const int gVertexNormalData[] = {
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,
        0,  1,  0,

        0, -1,  0,
        0, -1,  0,
        0, -1,  0,
        0, -1,  0,
        0, -1,  0,
        0, -1,  0,

        1, 0,  0,
        1, 0,  0,
        1, 0,  0,
        1, 0,  0,
        1, 0,  0,
        1, 0,  0,

        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,
        -1, 0,  0,

        0,  0,  1, 
        0,  0,  1, 
        0,  0,  1, 
        0,  0,  1, 
        0,  0,  1, 
        0,  0,  1, 

        0,  0, -1, 
        0,  0, -1, 
        0,  0, -1, 
        0,  0, -1, 
        0,  0, -1, 
        0,  0, -1 
    };

    // VBO init
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
            GL_STATIC_DRAW);
    // normal buffer init

    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexNormalData), gVertexNormalData,
            GL_STATIC_DRAW);

    // link shaders from the project directory
    GLuint programID = createShaderProgram(
            loadShader("src/shaders/vertexShader.glslx", GL_VERTEX_SHADER),
            loadShader("src/shaders/fragmentShader.glslx", GL_FRAGMENT_SHADER));
    glUseProgram(programID);

    glm::mat4 mvp;
    GLuint matrixID = glGetUniformLocation(programID, "MVP");

    // init own input handler
    inputHandlerInit(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // render settings 
    glEnable(GL_DEPTH_TEST); // Enable depth test
    glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
    glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera

#ifndef _DEBUG
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw to the screen
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,        // match layout in vertex shader
                3,        // size
                GL_INT,// type
                GL_FALSE, // normalized?
                0,        // stride
                (void *)0 // array buffer offset
                );
        // normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(
                1,
                3,
                GL_INT,
                GL_FALSE,
                0,
                (void *)0
                );

        // apply mvp matrix
        getMVPMatrix(mvp);
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup

    // VBO cleanup
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();
    return 0;
}
