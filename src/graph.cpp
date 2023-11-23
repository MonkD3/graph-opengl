#include "headers/graph.hpp"
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cstddef>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

#include "headers/io.hpp"

void Graph::read_edgelist_file(const char* fname){

    readFile(fname, &n_vtx, &n_edges, rowstart, adj, vtxw, adjw);

    double max_w = 0.0;
    wDeg.resize(n_vtx);
    for (size_t i = 0; i < n_vtx; i++) wDeg[i] = 0.0;
    // Compute weighted degrees
    for (size_t i = 0; i < n_vtx; i++){
        wDeg[i] = 0.0;
        for (size_t j = rowstart[i]; j < rowstart[i+1]; j++){
            const double neigw = adjw[j];
            max_w = std::max(max_w, neigw);
            wDeg[i] += neigw;
        }
    }

    
    float max_wdeg = 0.0f;
    for (size_t i = 0; i < n_vtx; i++) max_wdeg = std::max(max_wdeg, wDeg[i]);

    for (size_t i = 0; i < n_edges; i++) adjw[i] /= max_w; // Normalize the weights
    for (size_t i = 0; i < n_vtx; i++) wDeg[i] /= 2.0f * max_wdeg;

    pos.reserve(2*n_vtx);
    colors.reserve(3*n_vtx);
    for (size_t i = 0; i < n_vtx; i++){
        pos[2*i] = -1.0f + 2.0f*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        pos[2*i+1] = -1.0f + 2.0f*static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }
}

void Graph::read_partition_file(const char* fname){
    std::fstream fh(fname);
    if (!fh.is_open()) {
        printf("File : %s couldn't be opened\n", fname);
        return;
    }

    while (!fh.eof()) {
        std::string line;
        std::getline(fh, line);

        std::vector<int> res;
        std::string cell;

        std::stringstream  lineStream(line);
        while (std::getline(lineStream, cell, ',')) res.push_back(std::stoi(cell));
        
        // Should only be triggered once
        if (hierarchies.size() < res.size()) hierarchies.resize(res.size());

        for (size_t i = 0; i < res.size(); i++) hierarchies[i].push_back(res[i]);
    }

    n_hierarchy = hierarchies.size();
    curr_hierarchy = n_hierarchy - 1;
}

Graph::Graph(const char * fedges, const char * fpart){
    read_edgelist_file(fedges);
    read_partition_file(fpart);
}

void Graph::step(){
    //  Attraction - repulsion - gravity model 
    float *dp = new float[2*n_vtx];
    for (size_t i = 0; i < 2*n_vtx; i++) dp[i] = 0.0f;
    const float dt = 1.0f/20.0f;

    // Gravity force : Pull back every node towards the center of the canvas 
    const float Fg = 0.1f;
    for (size_t i = 0; i < n_vtx; i++){
        const float px = pos[2*i];
        const float py = pos[2*i+1];
        const float norm = std::hypot(px, py);
        dp[2*i]   -= px / norm * Fg;
        dp[2*i+1] -= py / norm * Fg;
    }

    // Repulsion force : simply use the reverse of the distance between nodes
    const float Fr = 0.10f;
    // const float Fr = 0.15f;

    for (size_t i = 0; i < n_vtx-1; i++){
        const float ix = pos[2*i];
        const float iy = pos[2*i+1];
        for (size_t j = i+1; j < n_vtx; j++){
            const float jx = pos[2*j];
            const float jy = pos[2*j+1];
            // Direction from j to i
            const float vx = ix-jx;
            const float vy = iy-jy;
            const float dist = vx*vx + vy*vy; 

            dp[2*i] += Fr*vx/dist; dp[2*j] -= Fr*vx/dist;
            dp[2*i+1] += Fr*vy/dist; dp[2*j+1] -= Fr*vy/dist;
        }
    }

    // Attraction forces : Vertices linked to each other attract themselves
    //const float Fa = 0.05f;
    const float Fa = 2.00;
    for (size_t i = 0; i < n_vtx; i++){
        for (size_t j = rowstart[i]; j < rowstart[i+1]; j++){
            const size_t neig = adj[j];
            const double neigw = adjw[j];
            
            const float ix = pos[2*i];
            const float iy = pos[2*i+1];
            const float neigx = pos[2*neig];
            const float neigy = pos[2*neig+1];
            // Direction from j to i
            const float vx = ix-neigx;
            const float vy = iy-neigy;
            const float dist = std::hypot(vx, vy);

            dp[2*i] -= Fa*neigw*vx/dist; dp[2*neig] += Fa*neigw*vx/dist;
            dp[2*i+1] -= Fa*neigw*vy/dist; dp[2*neig+1] += Fa*neigw*vy/dist;
        }
    }

    for (size_t i = 0; i < 2*n_vtx; i++) pos[i] += dt*dp[i];
    delete [] dp;
    return;
}
