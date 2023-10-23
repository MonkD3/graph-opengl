#include "headers/glad/gl.h"
#include <GLFW/glfw3.h>
#include "headers/shader_functions.hpp"
#include "headers/graph.hpp"
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
float aspect_ratio = 1.0f;
std::array<float, 4> rotation;
float translationX = 0., translationY = 0.;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) paused = !paused; 
}

// Callback on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
    aspect_ratio = ((float) height)/width;
}  

// Callback on scrolling with mouse or pad. 
//   - Classical mouse will only provide yoffset 
//   - pad may provide x and y offsets
//
//   On the mouse from my PC it's only +1 or -1
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    zoom += 0.03f * (float) yoffset;
    zoom = std::max(0.0f, zoom);
}

// Callback on cursor movement
void cursorCallback(GLFWwindow* window, double x, double y){
    static bool first = true;
    static double mouseX=0., mouseY=0.;

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
    // Compiling shader for lines
    GLuint vertexShaderLines = loadShaders("./shaders/vertexShaderLines.glsl", GL_VERTEX_SHADER);

    // ... And fragments
    GLuint fragmentShaderLines = loadShaders("./shaders/fragmentShaderLines.glsl", GL_FRAGMENT_SHADER);

    // .. now create a program using these shaders
    GLuint shaderLinesProgram = loadProgram(vertexShaderLines, fragmentShaderLines);
    // And as we already linked the program we no longer need the shaders
    glDeleteShader(vertexShaderLines);
    glDeleteShader(fragmentShaderLines);

    // Compiling shader for vertices
    GLuint vertexShader = loadShaders("./shaders/vertexShader.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShaders("./shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
    GLuint shaderProgram = loadProgram(vertexShader, fragmentShader);

    // And as we already linked the program we no longer need the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Graph g("./graphs/flups_512cpu.csv", "./graphs/flups_512cpu_part.csv");
    
    // ============= OpenGL Objects related to Vertices ===================

    // Create the vertex array object
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO
    GLuint VBO[3];
    glGenBuffers(3, VBO);
    GLuint vtx = VBO[0], color = VBO[1], pos = VBO[2];
    g.VAO = VAO;
    g.posVBO = pos;

    // Create the EBO 
    GLuint EBO;
    glGenBuffers(1, &EBO);

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


    std::vector<float> colors;
    generate_colors(colors, 3*g.n_vtx);
    
    for (size_t i = 0; i < g.n_vtx; i++) {
        int community = g.hierarchies[g.curr_hierarchy][i];
        g.colors[3*i]   = colors[3*community];
        g.colors[3*i+1] = colors[3*community+1];
        g.colors[3*i+2] = colors[3*community+2];
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_READ);

    // Copy vertices data into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, vtx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute related to the vtx buffer
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute related to the pos buffer
    glBindBuffer(GL_ARRAY_BUFFER, g.posVBO);
    glBufferData(GL_ARRAY_BUFFER, g.n_vtx*2*sizeof(float), &g.pos[0], GL_STREAM_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    // Attribute related to the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, g.n_vtx*3*sizeof(float), &g.colors[0], GL_STREAM_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3*sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    // ================ OpenGL objects related to edges ===================
    GLuint VAOlines;
    glGenVertexArrays(1, &VAOlines);
    glBindVertexArray(VAOlines);
   
    GLuint EBOlines;
    glGenBuffers(1, &EBOlines);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOlines);
    

    unsigned int *edges = new unsigned int[2*g.n_edges];
    int k = 0;
    for (unsigned int i = 0; i < g.n_vtx; i++){
        for (int j = g.rowstart[i]; j < g.rowstart[i+1]; j++){
            unsigned int neig = g.adj[j];
            if (neig > i){
                edges[k++] = i;
                edges[k++] = neig;
            }
        }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*g.n_edges*sizeof(unsigned int), edges, GL_STATIC_READ);

    glBindBuffer(GL_ARRAY_BUFFER, g.posVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );

    // FPS seems to be set at 60 for my laptop
    double t = 0.0;
    double t_end = 0.0, t_start = 0.0;
    while(!glfwWindowShouldClose(window)) {
        t_start = glfwGetTime();
        glfwPollEvents();    
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Compute world matrix
        double s = 0.0f; // sinf(0.2f*t*M_PI);
        double c = 1.0f; // cosf(0.2f*t*M_PI);
        rotation[0] = zoom * aspect_ratio * c; rotation[1] = zoom * -s;
        rotation[2] = zoom * aspect_ratio * s; rotation[3] = zoom * c;

        // Draw lines
        glUseProgram(shaderLinesProgram);
        glUniformMatrix2fv(glGetUniformLocation(shaderLinesProgram, "rotation"), 1, false, &rotation[0]);
        glUniform2f(glGetUniformLocation(shaderLinesProgram, "translation"), translationX, translationY);
        glBindVertexArray(VAOlines);
        glDrawElements(GL_LINES, g.n_edges, GL_UNSIGNED_INT, 0);

        // Draw nodes
        glUseProgram(shaderProgram);
        glUniformMatrix2fv(glGetUniformLocation(shaderProgram, "rotation"), 1, false, &rotation[0]);
        glUniform2f(glGetUniformLocation(shaderProgram, "translation"), translationX, translationY);
        g.draw();

        glfwSwapBuffers(window);
        if (!paused){
            g.step();
            t_end = glfwGetTime();
            t += t_end - t_start;
        }
    }

    delete [] edges;
    glfwTerminate();
    return EXIT_SUCCESS;
}
