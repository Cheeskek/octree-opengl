#include "headers/shader.hpp"
#include "headers/inputHandler.hpp"
#include "headers/engine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/trigonometric.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifdef _DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

namespace {
    glm::mat4 mvp;
    GLuint matrixID;
    glm::mat4 vp;
}

void drawCube(int x, int y, int z, int scale) {
    mvp = vp * glm::scale(glm::translate(glm::mat4(1), glm::vec3(x, y, z)), glm::vec3(scale));
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(
            GL_TRIANGLE_STRIP,
            14,
            GL_UNSIGNED_SHORT,
            (void*)0
            );
}

int main() {
    // glew init
    glewExperimental = true;
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
    }

    // window hints
    glfwWindowHint(GLFW_SAMPLES, 1);               // 1x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy;
    glfwWindowHint(GLFW_OPENGL_PROFILE,
            GLFW_OPENGL_CORE_PROFILE); // no old opengl

    DEBUG_ONLY(glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE));

    // Open a window and create its OpenGL context
    GLFWwindow *window;
    unsigned WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "opengl_window",
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

    // поменял -1 на 0, тк криво отображалось
    // на нормалях так менять нельзя, сломается (конечно, пока теней нет совсем, ну ладно)
    // cube vertex data
    static const short gVertexBufferData[] = {
        0, 0, 0,
        0, 0, 1,
        0, 1, 0,
        0, 1, 1,
        1, 0, 0,
        1, 0, 1,
        1, 1, 0,
        1, 1, 1
    };

    static const unsigned short gVertexIndexBuffer[] = {
        0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5
    };

    // VBO init
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData,
            GL_STATIC_DRAW);
    // create pointer
    glVertexAttribPointer(
            0,        // match layout in vertex shader
            3,        // size
            GL_SHORT,   // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
                );
        
    // cube element array buffer
    GLuint vertexElementBuffer;
    glGenBuffers(1, &vertexElementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gVertexIndexBuffer), gVertexIndexBuffer,
            GL_STATIC_COPY);

    // link shaders from the project directory
    GLuint programID = createShaderProgram({
            loadShader("src/shaders/vertexShader.glslx", GL_VERTEX_SHADER),
            loadShader("src/shaders/geometryShader.glslx", GL_GEOMETRY_SHADER),
            loadShader("src/shaders/fragmentShader.glslx", GL_FRAGMENT_SHADER)
            });
    glUseProgram(programID);

    matrixID = glGetUniformLocation(programID, "MVP");

    // init own input handler
    inputHandlerInit(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // render settings 
    glEnable(GL_DEPTH_TEST); // Enable depth test
    glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
    glEnable(GL_CULL_FACE); // Cull triangles which normal is not towards the camera

    // init world tree
    initTree(drawCube, 10);


#ifndef _DEBUG
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_TRUE);
#endif

    bool drawHelpCube = true;

    bool fileIBefore = false, fileOBefore = false;

#ifdef _DEBUG
    double lastTime = glfwGetTime();
    int nbFrames = 0;
#endif

    while (!glfwWindowShouldClose(window)) {
#ifdef _DEBUG
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
#endif
        glfwPollEvents();
        
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            drawHelpCube = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            drawHelpCube = false;
        }

        if (fileIBefore && glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            fileIBefore = false;
            printf("File input, write input file name\n");
            fflush(stdout);
            std::string fileName;
            getline(std::cin, fileName);
            if (treeFileInput(fileName.c_str())) {
                fprintf(stderr, "Error when reading file. Check file name\n");
            }
            else {
                printf("Success!\n");
            }
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            getchar();
        }
        else if (fileOBefore && glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            fileOBefore = false;
            printf("File output, write output file name\n");
            fflush(stdout);
            std::string fileName;
            getline(std::cin, fileName);
            if (treeFileOutput(fileName.c_str())) {
                fprintf(stderr, "Error when writing to file. Check file name\n");
            }
            else {
                printf("Success!\n");
            }
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            getchar();
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            fileIBefore = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            fileOBefore = true;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw to the screen
        glEnableVertexAttribArray(0);
        // apply vp matrix
        getVPMatrix(vp);

        if (drawHelpCube) {
            drawCube(getCameraCubeX(), getCameraCubeY(), getCameraCubeZ(), getCameraCubeScale());
        }
        
        //drawCube(0, 0, 0, 64);

        // draw world tree
        drawTree();
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
    }

    // Cleanup

    // VBO cleanup
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();
    return 0;
}
