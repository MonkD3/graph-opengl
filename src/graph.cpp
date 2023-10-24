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



void generate_colors(std::vector<float>& colors, size_t n){
    colors.reserve(3*n);
    for (int i = 0; i < 3*n; i++) colors[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void Graph::read_edgelist_file(const char* fname){
    FILE* fh = fopen(fname, "r");
    if (fh == NULL){
        printf("File : %s couldn't be opened\n", fname);
        return;
    }
    
    // u for undirected, d for directed
    char gt;
    fscanf(fh, "# type:%c edges:%ld", &gt, &n_edges);
    if (gt == 'u') n_edges *= 2; // We encode only one edge per line in case of undirected

    std::vector<Edge> edges;

    if (gt == 'd') {
        for (size_t i = 0; i < n_edges; i++){
            Edge e_direct;
            fscanf(fh, "%ld,%ld,%lf", &e_direct.src, &e_direct.dest, &e_direct.w);
            edges.push_back(e_direct);
        }
    }
    else if (gt == 'u') {
        for (size_t i = 0; i < n_edges; i+=2){
            Edge e_direct, e_indirect;
            fscanf(fh, "%ld,%ld,%lf", &e_direct.src, &e_direct.dest, &e_direct.w);
            e_indirect.src = e_direct.dest;
            e_indirect.dest = e_direct.src;
            e_indirect.w = e_direct.w;
            edges.push_back(e_direct);
            edges.push_back(e_indirect);
        }
    }
    else {
        printf("Unsupported graph type, use type:u or type:d");
    }
    fclose(fh);

    std::sort(edges.begin(), edges.end(), EdgeComp() );

    // Collapse multiple edges
    size_t put_idx = 0;
    for (size_t i = 1; i < n_edges; i++){
        // TODO: check for how to handle directed graphs
        if (edges[i] == edges[put_idx]) edges[put_idx].w += edges[i].w;
        else edges[++put_idx] = edges[i];
    }
    n_edges = put_idx + 1;

    // NOTE: We assume that vertices are numeroted from 0 to n_vtx-1
    n_vtx = std::max(edges[n_edges - 1].src, edges[n_edges-1].dest) + 1;

    rowstart.reserve(n_vtx + 1);
    adj.reserve(n_edges);
    adjw.reserve(n_edges);

    wDeg.resize(n_vtx);
    for (size_t i = 0; i < n_vtx; i++) wDeg[i] = 0.0;

    size_t curr_edge = 0;
    rowstart.push_back(0.0);
    double max_w = 0.0;
    for (size_t i = 0; i < n_vtx; i++){
        int degree = 0;
        while ((curr_edge < n_edges) && (edges[curr_edge].src == i)) {
            max_w = std::max(max_w, edges[curr_edge].w);
            adj.push_back(edges[curr_edge].dest);
            adjw.push_back(edges[curr_edge].w);
            wDeg[edges[curr_edge].dest] += (float) edges[curr_edge].w;
            curr_edge++;
            degree++;
        }
        rowstart.push_back(rowstart[i] + degree);
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

    float *dp = new float[2*n_vtx];
    for (size_t i = 0; i < 2*n_vtx; i++) dp[i] = 0.0f;

    // Gravity force : Pull back every node towards the center of the canvas 
    const float Fg = 1.0f;
    const float dt = 1.0f/20.0f;
    for (size_t i = 0; i < n_vtx; i++){
        const float px = pos[2*i];
        const float py = pos[2*i+1];
        const float norm = std::hypot(px, py);
        dp[2*i]   -= Fg * px / norm * Fg;
        dp[2*i+1] -= Fg * py / norm * Fg;
    }

    // Repulsion force : simply use the reverse of the distance between nodes
    float Fr = 0.1f;

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
    // const float Fa = 0.05f;
    const float Fa = 0.05f;
    for (size_t i = 0; i < n_vtx; i++){
        for (size_t j = rowstart[i]; j < rowstart[i+1]; j++){
            size_t neig = adj[j];
            double neigw = adjw[j];
            
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
}
