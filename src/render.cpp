#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/graph.hpp"
#include <cmath>
#include <cstddef>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

GLint WIDTH;
GLint HEIGHT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    WIDTH=width;
    HEIGHT=height;
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

    // Array of vertices
    Vertex* vtx = new Vertex[6];

    // Initialize their positions
    vtx[0].x = -0.5f; vtx[0].y = -0.5f;
    vtx[1].x =  0.5f; vtx[1].y = -0.5f;
    vtx[2].x = -0.5f; vtx[2].y =  0.5f;
    vtx[3].x =  0.6f; vtx[3].y = -0.5f;
    vtx[4].x = -0.4f; vtx[4].y =  0.5f;
    vtx[5].x =  0.6f; vtx[5].y =  0.5f;

    // Initialize their colors
    vtx[0].c[0] = 0.5f;vtx[0].c[1] = 0.0f;vtx[0].c[2] = 0.2f;
    vtx[1].c[0] = 0.7f;vtx[1].c[1] = 0.0f;vtx[1].c[2] = 0.2f;
    vtx[2].c[0] = 1.0f;vtx[2].c[1] = 0.0f;vtx[2].c[2] = 0.2f;
    vtx[3].c[0] = 1.0f;vtx[3].c[1] = 0.0f;vtx[3].c[2] = 0.5f;
    vtx[4].c[0] = 1.0f;vtx[4].c[1] = 0.0f;vtx[4].c[2] = 0.7f;
    vtx[5].c[0] = 1.0f;vtx[5].c[1] = 0.0f;vtx[5].c[2] = 1.0f;

    // Compute the initial phase
    std::vector<float> phi = {
        std::atan2(vtx[0].x, vtx[0].y),
        std::atan2(vtx[1].x, vtx[1].y),
        std::atan2(vtx[2].x, vtx[2].y),
        std::atan2(vtx[3].x, vtx[3].y),
        std::atan2(vtx[4].x, vtx[4].y),
        std::atan2(vtx[5].x, vtx[5].y)
    };

    // Compute the radius
    std::vector<float> r = {
        hypotf(vtx[0].x, vtx[0].y),
        hypotf(vtx[1].x, vtx[1].y),
        hypotf(vtx[2].x, vtx[2].y),
        hypotf(vtx[3].x, vtx[3].y),
        hypotf(vtx[4].x, vtx[4].y),
        hypotf(vtx[5].x, vtx[5].y)
    };

    // Create the vertex buffer object
    GLuint VBO;
    glGenBuffers(1, &VBO);   // Generate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Make it the current openGL object
    // Add data to the current buffer 
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vertex), vtx, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(
            0,                  // must math the layout in the shader
            2,                  // size
            GL_FLOAT,           // Type
            GL_FALSE,           // Are the coords normalized ?
            sizeof(Vertex),     // Stride
            (void*)0            // offset
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            1,                       // must math the layout in the shader
            3,                       // size
            GL_FLOAT,                // Type
            GL_TRUE,                 // Are the coords normalized ?
            sizeof(Vertex),         // Stride
            (void*)(offsetof(Vertex, c)) // offset
    );
    glEnableVertexAttribArray(1);

    // FPS seems to be set at 60 for my laptop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();    
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float scale[4] = {
            ((float) HEIGHT)/WIDTH , 0.0f,
            0.0f,                    1.0f
        };

        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, "scale"), 1, false, scale);

        double t = glfwGetTime();
        for (int i = 0; i < 6; i++){
            vtx[i].x = (float) r[i]*std::cos(0.05*M_PI*t + phi[i]);
            vtx[i].y = (float) r[i]*std::sin(0.05*M_PI*t + phi[i]);
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Make it the current openGL object
                                             // Add data to the current buffer 
        glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vertex), vtx, GL_STREAM_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    delete [] vtx;
    glfwTerminate();
    return EXIT_SUCCESS;
}
