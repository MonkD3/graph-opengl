#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/objects.hpp"
#include <cmath>
#include <cstddef>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <array>

bool leftButtonPressed = false;

float zoom = 1.0f;
float scale = 1.0f;
std::array<float, 4> rotation;


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
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
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

    // Create the vertex array object
    GLuint VAO[2];
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);

    // Create the vertex buffer object
    GLuint VBO[2];
    glGenBuffers(2, VBO);   // Generate the buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);  // Make it the current openGL object
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

    Vertex *dv = new Vertex[10];
    Disk d(0.7, 0.7);
    d.v = dv;
    d.draw();
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);  // Make it the current openGL object
    // Add data to the current buffer 
    glBufferData(GL_ARRAY_BUFFER, 10*sizeof(Vertex), dv, GL_STATIC_DRAW);

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

        double t = glfwGetTime();
        double s = sinf(0.05f*t*M_PI);
        double c = cosf(0.05f*t*M_PI);
        rotation[0] = zoom * scale * c; rotation[1] = zoom * -s;
        rotation[2] = zoom * scale * s; rotation[3] = zoom * c;

        GLint shaderProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, "rotation"), 1, false, &rotation[0]);

        glBindVertexArray(VAO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);  
        glBufferData(GL_ARRAY_BUFFER, 6*sizeof(Vertex), vtx, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);  
        glBufferData(GL_ARRAY_BUFFER, 10*sizeof(Vertex), dv, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 10);

        glfwSwapBuffers(window);
    }

    delete [] vtx;
    glfwTerminate();
    return EXIT_SUCCESS;
}
