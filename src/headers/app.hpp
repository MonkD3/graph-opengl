#ifndef __APP_HPP
#define __APP_HPP 

#include "graph.hpp"
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <array>

class App {

    public:
        // OpenGL objects
        GLFWwindow* window;
        GLuint VAO[2]; // 0: VAO for nodes, 1: VAO for edges
        GLuint VBO[4]; // 0: base shape, 1 : color, 2: position, 3: size
        GLuint EBO[1]; // EBO for lines
        GLuint vertexShaderLines, fragmentShaderLines, lineShaderProgram;
        GLuint vertexShader, fragmentShader, nodeShaderProgram;

        // User Interactions
        bool rightButtonPressed = false;
        bool paused = false;
        float translationX = 0.0f; // User pan in X
        float translationY = 0.0f; // User pan in Y

        float mouseX = 0.0f; // Last recorded mouse pos in X
        float mouseY = 0.0f; // Last recorded mouse pos in Y

        // Matrix transformation of the scene
        float zoom = 1.0f;
        float aspectRatio = 1.0f;
        std::array<float, 4> sceneMVP = {0.0f, 0.0f, 0.0f, 0.0f};
        std::vector<float> colors;

        // Graph 
        Graph* g;

        // Constructor
        App(const char* fedges, const char* fpart);

        // Initialize the app 
        int init();
        int compileShaders();
        void loadOpenGLObjects();

        // Draw a frame
        void draw();

        // Update colors 
        void generateColors();
        void updateColors();

        // scene 
        void computeTransform();

};
#endif // __APP_HPP
