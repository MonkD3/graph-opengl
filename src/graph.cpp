#include "headers/graph.hpp"
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <cstddef>
#include <vector>

#include <GLFW/glfw3.h>

Graph::Graph(const char * fname){
    FILE* fh = fopen(fname, "r");
    if (fh == NULL) return;
    
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
    n_vtx = edges[n_edges - 1].src + 1;

    rowstart.reserve(n_vtx + 1);
    adj.reserve(n_edges);
    adjw.reserve(n_edges);

    size_t curr_edge = 0;
    rowstart.push_back(0.0);
    for (size_t i = 0; i < n_vtx; i++){
        int degree = 0;
        while ((curr_edge < n_edges) && (edges[curr_edge].src == i)) {
            adj.push_back(edges[curr_edge].dest);
            adjw.push_back(edges[curr_edge].w);
            curr_edge++;
            degree++;
        }
        rowstart.push_back(rowstart[i] + degree);
    }

    pos.reserve(2*n_vtx);
    for (int i = 0; i < n_vtx; i++){
        pos[2*i] = 3.0f*cos(M_PIf * 2.0f * (float) i / n_vtx);
        pos[2*i+1] = 3.0f*sin(M_PIf * 2.0f * (float) i / n_vtx);
    }
}

void Graph::step(){

    // Gravity force : Pull back every node towards the center of the canvas 
    const float Fg = 0.2f;
    const float dt = 1.0f/30.0f;
    for (int i = 0; i < n_vtx; i++){
        const float px = pos[2*i];
        const float py = pos[2*i+1];
        const float norm = std::hypot(px, py);
        pos[2*i]   -= dt * px / norm * Fg;
        pos[2*i+1] -= dt * py / norm * Fg;
    }

    // Repulsion force : simply use the reverse of the distance between nodes
    float Fr = 0.1f;
    float *dp = new float[2*n_vtx];
    for (int i = 0; i < 2*n_vtx; i++) dp[i] = 0.0f;

    for (int i = 0; i < n_vtx-1; i++){
        const float ix = pos[2*i];
        const float iy = pos[2*i+1];
        for (int j = i+1; j < n_vtx; j++){
            const float jx = pos[2*j];
            const float jy = pos[2*j+1];
            // Direction from j to i
            const float vx = ix-jx;
            const float vy = iy-jy;
            const float dist2 = vx*vx + vy*vy;

            dp[2*i] += vx/dist2; dp[2*j] -= vx/dist2;
            dp[2*i+1] += vy/dist2; dp[2*j+1] -= vy/dist2;
        }
    }
    for (int i = 0; i < 2*n_vtx; i++) pos[i] += dt*Fr*dp[i];

    // Attraction forces : Vertices linked to each other attract themselves
    float Fa = 0.05f;
    for (int i = 0; i < 2*n_vtx; i++) dp[i] = 0.0f;
    for (int i = 0; i < n_vtx; i++){
        for (int j = rowstart[i]; j < rowstart[i+1]; j++){
            size_t neig = adj[j];
            double neigw = adjw[j];
            
            const float ix = pos[2*i];
            const float iy = pos[2*i+1];
            const float neigx = pos[2*neig];
            const float neigy = pos[2*neig+1];
            // Direction from j to i
            const float vx = ix-neigx;
            const float vy = iy-neigy;
            const float dist2 = vx*vx + vy*vy;

            dp[2*i] -= vx/dist2; dp[2*neig] += vx/dist2;
            dp[2*i+1] -= vy/dist2; dp[2*neig+1] += vy/dist2;
        }
    }

    for (int i = 0; i < 2*n_vtx; i++) pos[i] += dt*Fa*dp[i];
    delete [] dp;
}

void Graph::draw(){
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * n_vtx, &pos[0], GL_STREAM_DRAW);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, n_vtx);
}
