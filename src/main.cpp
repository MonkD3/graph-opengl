#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/app.hpp"
#include "headers/graph.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <array>

App app("./graphs/edges_FD_scheme.csv", "./graphs/edges_FD_part.csv");

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) app.paused = !app.paused; 
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        app.g->curr_hierarchy = std::min(app.g->curr_hierarchy + 1, app.g->n_hierarchy - 1); 
        app.updateColors();
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        app.g->curr_hierarchy = std::max(app.g->curr_hierarchy - 1, 0); 
        app.updateColors();
    }
}

// Callback on window resize
void framebufferSizeCallback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    app.aspectRatio = ((float) height)/width;
}  

// Callback on scrolling with mouse or pad. 
//   - Classical mouse will only provide yoffset 
//   - pad may provide x and y offsets
//   On the mouse from my PC it's only +1 or -1
//
//   Compute a translation-scale-translation so that the zoom is done 
//   towards the cursos rather then the center of the scene. 
//   2d computation looks like this :
//  translation = (translation towards center) (scaling) (translation towards mouse) (current translation)
//      [tx]   [1 0 mx] [zx 0  0] [1 0 -mx] [tx]
//      [ty] = [0 1 my] [0  zy 0] [0 1 -mx] [ty]
//      [1]    [0 0 1 ] [0  0  1] [0 0   1] [1]
//  where tx/ty are the translation in x/y, zx/zy are the zoom and mx/my are the position of the mouse.
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    double xpos, ypos;
    int width, height;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);

    const float zoomStrength = 1.5f;
    float newZoom;
    if (yoffset > 0) newZoom = zoomStrength; // zooming in
    else newZoom = 1.f / zoomStrength;       // zomming out

    newZoom = std::max(0.001f, newZoom * app.zoom);
    const float tranScaling = newZoom / app.zoom;
    app.zoom = newZoom;

    xpos = (2.f*xpos - width)/width;
    ypos = (height - 2.f*ypos)/height;
    app.translationX = (app.translationX - xpos) * tranScaling + xpos;
    app.translationY = (app.translationY - ypos) * tranScaling + ypos;
}

// Callback on cursor movement
void cursorCallback(GLFWwindow* window, double x, double y){
    static bool first = true;
    static double mouseX = 0., mouseY = 0.;

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
