#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/app.hpp"
#include "headers/graph.hpp"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <array>

App app("./graphs/flups_512cpu.csv", "./graphs/flups_512cpu_part.csv");

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) app.paused = !app.paused; 
}

// Callback on window resize
void framebufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    app.aspectRatio = ((float) height)/width;
}  

// Callback on scrolling with mouse or pad. 
//   - Classical mouse will only provide yoffset 
//   - pad may provide x and y offsets
//
//   On the mouse from my PC it's only +1 or -1
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    app.zoom += 0.03f * (float) yoffset;
    app.zoom = std::max(0.001f, app.zoom);
}

// Callback on cursor movement
void cursorCallback(GLFWwindow* window, double x, double y){
    static bool first = true;
    static double mouseX=0., mouseY=0.;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    x = (2.f*x - width)/width;
    y = (height - 2.f*y)/height;
    if (first || !app.rightButtonPressed){
        mouseX = x;
        mouseY = y;
        first = false;
        return;
    }

    app.translationX -= mouseX - x;
    app.translationY -= mouseY - y;

    mouseX = x;
    mouseY = y;
}

// Callback on mouse buttons
void mouseCallback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            app.rightButtonPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            app.rightButtonPressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        
    }
}

int main(int argc, char** argv){

    app.init();

    glfwSetFramebufferSizeCallback(app.window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(app.window, mouseCallback);
    glfwSetCursorPosCallback(app.window, cursorCallback);
    glfwSetScrollCallback(app.window, scrollCallback);
    glfwSetKeyCallback(app.window, keyCallback);

    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // FPS seems to be set at 60 for my laptop
    while(!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();    
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        app.draw();
        glfwSwapBuffers(app.window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
