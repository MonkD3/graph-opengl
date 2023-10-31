#include "headers/app.hpp"
#include "headers/shader_functions.hpp"
#include <cstdlib>
#include <stdio.h>

App::App(){}

int App::init(const char* fedges, const char* fpart){
    if (!glfwInit()){
        printf("Failed to initialize glfw\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 1); // 1x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Graph Visualisation", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return -1;
    }

    g = new Graph(fedges, fpart);

    compileShaders();
    loadOpenGLObjects();

    return 0;
}

int App::compileShaders(){
    // Compiling shader for lines
    vertexShaderLines = loadShaders("./shaders/vertexShaderLines.glsl", GL_VERTEX_SHADER);

    // ... And fragments
    fragmentShaderLines = loadShaders("./shaders/fragmentShaderLines.glsl", GL_FRAGMENT_SHADER);

    // .. now create a program using these shaders
    lineShaderProgram = loadProgram(vertexShaderLines, fragmentShaderLines);
    // And as we already linked the program we no longer need the shaders
    glDeleteShader(vertexShaderLines);
    glDeleteShader(fragmentShaderLines);

    // Compiling shader for vertices
    vertexShader = loadShaders("./shaders/vertexShader.glsl", GL_VERTEX_SHADER);
    fragmentShader = loadShaders("./shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
    nodeShaderProgram = loadProgram(vertexShader, fragmentShader);

    // And as we already linked the program we no longer need the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return 0;
}

void App::loadOpenGLObjects(){

    // Create the vertex array object
    glGenVertexArrays(2, VAO);
    glBindVertexArray(VAO[0]);

    // Create the VBO
    glGenBuffers(4, VBO);
    GLuint vtx = VBO[0], color = VBO[1], pos = VBO[2], size = VBO[3];

    // Base shape for drawing the nodes
    // This does not need an EBO :
    //      With an EBO we would have 8 floats for positions 
    //                                + 6 uint for indexes
    //      Totaling (8 + 6)*32 bits = 14 * 32 bits 
    // Without EBO we have 12*32 bits and a better access pattern.
    float vertices[12] = {
        // 1st tri 
        1.0f,  1.0f,
        1.0f, -1.0f,
       -1.0f,  1.0f,
        // 2nd tri
        1.0f, -1.0f,
       -1.0f, -1.0f,
       -1.0f,  1.0f
    };

    generateColors();
    for (size_t i = 0; i < g->n_vtx; i++) {
        int community = g->hierarchies[g->curr_hierarchy][i];
        g->colors[3*i]   = colors[3*community];
        g->colors[3*i+1] = colors[3*community+1];
        g->colors[3*i+2] = colors[3*community+2];
    }

    // Copy vertices data into the buffer
    glBindBuffer(GL_ARRAY_BUFFER, vtx);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute related to the vtx buffer
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute related to the pos buffer
    glBindBuffer(GL_ARRAY_BUFFER, pos);
    glBufferData(GL_ARRAY_BUFFER, g->n_vtx*2*sizeof(float), &g->pos[0], GL_STREAM_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    // Attribute related to the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, g->n_vtx*3*sizeof(float), &g->colors[0], GL_STATIC_READ);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 3*sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    // Attribute related to the size buffer
    glBindBuffer(GL_ARRAY_BUFFER, size);
    glBufferData(GL_ARRAY_BUFFER, g->n_vtx*sizeof(float), &g->wDeg[0], GL_STATIC_READ);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_TRUE, sizeof(float), (void*)0);
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

    // ================ OpenGL objects related to edges ===================
    glBindVertexArray(VAO[1]);
   
    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    

    unsigned int *edges = new unsigned int[2*g->n_edges];
    int k = 0;
    for (unsigned int i = 0; i < g->n_vtx; i++){
        for (int j = g->rowstart[i]; j < g->rowstart[i+1]; j++){
            unsigned int neig = g->adj[j];
            if (neig > i){
                edges[k++] = i;
                edges[k++] = neig;
            }
        }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*g->n_edges*sizeof(unsigned int), edges, GL_STATIC_READ);

    glBindBuffer(GL_ARRAY_BUFFER, pos);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    delete [] edges;
}

void App::draw(){

    if (!paused) {
        for (int i = 0; i < 10; i++) g->step();
    }
    computeTransform();
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * g->n_vtx, &g->pos[0], GL_STREAM_DRAW);

    // Draw lines
    glUseProgram(lineShaderProgram);
    glUniformMatrix2fv(glGetUniformLocation(lineShaderProgram, "rotation"), 1, false, &sceneMVP[0]);
    glUniform2f(glGetUniformLocation(lineShaderProgram, "translation"), translationX, translationY);
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_LINES, g->n_edges, GL_UNSIGNED_INT, 0);

    glUseProgram(nodeShaderProgram);
    glUniformMatrix2fv(glGetUniformLocation(nodeShaderProgram, "rotation"), 1, false, &sceneMVP[0]);
    glUniform2f(glGetUniformLocation(nodeShaderProgram, "translation"), translationX, translationY);

    glBindVertexArray(VAO[0]);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12, g->n_vtx);
}

void App::computeTransform(){
    const float s = 0.0f; 
    const float c = 1.0f;
    sceneMVP[0] = zoom * aspectRatio * c; sceneMVP[1] = zoom * -s;
    sceneMVP[2] = zoom * aspectRatio * s; sceneMVP[3] = zoom * c;
}

void App::generateColors(){
    colors.reserve(3*g->n_vtx);
    for (int i = 0; i < 3*g->n_vtx; i++) colors[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

}

void App::updateColors(){
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    
    for (size_t i = 0; i < g->n_vtx; i++) {
        int community = g->hierarchies[g->curr_hierarchy][i];
        g->colors[3*i]   = colors[3*community];
        g->colors[3*i+1] = colors[3*community+1];
        g->colors[3*i+2] = colors[3*community+2];
    }

    glBufferData(GL_ARRAY_BUFFER, g->n_vtx*3*sizeof(float), &g->colors[0], GL_STATIC_READ);
}
