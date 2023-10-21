#ifndef __GRAPH_HPP
#define __GRAPH_HPP
#include <vector>
#include <cstddef>
#include "glad/gl.h"
#include <GLFW/glfw3.h>

struct Edge {
    size_t src;
    size_t dest;
    double w;

    bool operator==(const Edge& other) const {
        bool eqDirect = (src == other.src) && (dest == other.dest);
        bool eqReverse = (dest == other.src) && (src == other.dest);
        return (eqDirect || eqReverse) && (w == other.w);
    }

    Edge& operator=(const Edge& other){
        src = other.src;
        dest = other.dest;
        w = other.w;
        return *this;
    }
};

struct EdgeComp {
    bool operator()(Edge& self, Edge& other) const {
        if (self.src == other.src){
            if (self.dest < other.dest) return true;
        }
        return (self.src < other.src);
    }
};

class Graph {
    public:
        size_t n_vtx;
        size_t n_edges;

        GLuint VAO; // GL vertex array object relating to this object
        GLuint posVBO;

        // CSR representation of the graph
        std::vector<size_t> rowstart;
        std::vector<size_t> adj;
        std::vector<double> adjw;

        // Position of the vertices
        std::vector<float> pos;

        Graph(const char* fname);

        // Compute one step of positionning algorithm
        void step();

        void draw();
};

#endif // __GRAPH_HPP
