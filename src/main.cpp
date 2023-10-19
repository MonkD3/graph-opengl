#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/objects.hpp"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <array>

bool leftButtonPressed = false;
bool paused = false;

float zoom = 1.0f;
float scale = 1.0f;
std::array<float, 4> rotation;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) paused = !paused; 
}

// Callback on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    scale = ((float) height)/width;
}  

// Callback on scrolling with mouse or pad. 
//   - Classical mouse will only provide yoffset 
//   - pad may provide x and y offsets
//
//   On the mouse from my PC it's only +1 or -1
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    zoom += 0.03f * (float) yoffset;
    zoom = fmax(0.1f, zoom);
}

// Callback on cursor movement
void cursorCallback(GLFWwindow* window, double x, double y){
    static bool first = true;
    static double mouseX=0., mouseY=0.;
    static double translationX = 0., translationY = 0.;

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    x = (2.f*x - width)/width;
    y = (height - 2.f*y)/height;
    if (first || !leftButtonPressed){
        mouseX = x;
        mouseY = y;
        first = false;
        return;
    }

    translationX -= mouseX - x;
    translationY -= mouseY - y;

    GLint shaderProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    glUniform2f(glGetUniformLocation(shaderProgram, "translation"), translationX, translationY);
    mouseX = x;
    mouseY = y;
}

// Callback on mouse buttons
void mouseCallback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftButtonPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            leftButtonPressed = false;
        }
    }
}

int main(int argc, char** argv){

    rotation[0] = 1.0f; rotation[1] = 0.0f;
    rotation[2] = 0.0f; rotation[3] = 1.0f;
    
    if (!glfwInit()){
        printf("Failed to initialize glfw\n");
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_SAMPLES, 1); // 1x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // Callback for the sizing of the window (allow resizing)
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }

    // Compiling shader for vertices
    GLuint vertexShader = loadShaders("./shaders/vertexShader.glsl", GL_VERTEX_SHADER);

    // ... And fragments
    GLuint fragmentShader = loadShaders("./shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);

    // .. now create a program using these shaders
    GLuint shaderProgram = loadProgram(vertexShader, fragmentShader);

    // We can finally use the program
    glUseProgram(shaderProgram);

    // And as we already linked the program we no longer need the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create the vertex array object
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO
    GLuint VBO[3];
    glGenBuffers(3, VBO);
    GLuint vtx = VBO[0], color = VBO[1], pos = VBO[2];

    // Create the EBO 
    GLuint EBO;
    glGenBuffers(1, &EBO);

    const int n_circles = 100;
    float vertices[8] = {
        1.0f,  1.0f,
        1.0f, -1.0f,
       -1.0f, -1.0f,
       -1.0f,  1.0f
    };
    GLuint indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    float* positions = new float[n_circles * 2];
    for (int i = 0; i < 2*n_circles; i++) positions[i] = -1.0f + 2.0f*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    float* colors = new float[n_circles * 3];
    for (int i = 0; i < 3*n_circles; i++) colors[i] = -1.0f + 2.0f*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Copy vertices data into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, vtx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute related to the vtx buffer
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute related to the pos buffer
    glBindBuffer(GL_ARRAY_BUFFER, pos);
    glBufferData(GL_ARRAY_BUFFER, n_circles*2*sizeof(float), positions, GL_STREAM_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    // Attribute related to the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, n_circles*3*sizeof(float), colors, GL_STREAM_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3*sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    // FPS seems to be set at 60 for my laptop
    double t = 0.0;
    double t_end = 0.0, t_start = 0.0;
    while(!glfwWindowShouldClose(window)) {
        t_start = glfwGetTime();
        glfwPollEvents();    
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double s = sinf(0.2f*t*M_PI);
        double c = cosf(0.2f*t*M_PI);
        rotation[0] = zoom * scale * c; rotation[1] = zoom * -s;
        rotation[2] = zoom * scale * s; rotation[3] = zoom * c;

        GLint shaderProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, "rotation"), 1, false, &rotation[0]);

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, n_circles);

        glfwSwapBuffers(window);
        if (!paused){
            t_end = glfwGetTime();
            t += t_end - t_start;
        }
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
