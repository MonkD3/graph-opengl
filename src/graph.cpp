#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}  

int main(int argc, char** argv){
    
    if (!glfwInit()){
        printf("Failed to initialize glfw\n");
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_SAMPLES, 2); // 2x antialiasing
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

    float vertices[] = {
    //    x      y     R     G     B
        -0.5f, -0.5f, 1.0f, 0.5f, 0.2f,
         0.5f, -0.5f, 0.0f, 0.5f, 0.2f,
        -0.5f,  0.5f, 1.0f, 0.5f, 0.2f,
         0.6f, -0.5f, 1.0f, 0.5f, 0.4f,
        -0.4f,  0.5f, 0.0f, 0.5f, 0.4f,
         0.6f,  0.5f, 1.0f, 0.5f, 0.4f
    };

    // Create the vertex buffer object
    GLuint VBO;
    glGenBuffers(1, &VBO);   // Generate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Make it the current openGL object
    // Add data to the current buffer 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(
            0,                  // must math the layout in the shader
            2,                  // size
            GL_FLOAT,           // Type
            GL_FALSE,           // Are the coords normalized ?
            5*sizeof(float),    // Stride
            (void*)0            // offset
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            1,                       // must math the layout in the shader
            3,                       // size
            GL_FLOAT,                // Type
            GL_TRUE,                 // Are the coords normalized ?
            5*sizeof(float),         // Stride
            (void*)(2*sizeof(float)) // offset
    );
    glEnableVertexAttribArray(1);

    // FPS seems to be set at 60 for my laptop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();    
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
