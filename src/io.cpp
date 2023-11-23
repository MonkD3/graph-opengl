#include "headers/io.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

int readUnweighted(FILE* fh, std::vector<size_t>& rowstart, std::vector<size_t>& adj, std::vector<double>& adjw, std::vector<size_t>& vtxw){
    const int len = 1024;
    char* line = (char*) malloc(sizeof(char)*len);

    size_t currVtx = 0;
    size_t currEdge = 0;

    rowstart[0] = 0;
    while (fgets(line, len, fh) != NULL) {
        char *token = strtok(line, " ");
        while(token){
            adj[currEdge] = (size_t) strtoul(token, NULL, 10) - 1;
            adjw[currEdge++] = (double) 1.0; 
            token = strtok(NULL, " ");
        };
        
        vtxw[currVtx++] = (size_t) 1;
        rowstart[currVtx] = currEdge;
    }

    free(line);
    return 0;
}

int readEdgeWeighted(FILE* fh, std::vector<size_t>& rowstart, std::vector<size_t>& adj, std::vector<double>& adjw, std::vector<size_t>& vtxw){
    const int len = 1024;
    char* line = (char*) malloc(sizeof(char)*len);

    size_t currVtx = 0;
    size_t currEdge = 0;

    rowstart[0] = 0;
    while (fgets(line, len, fh) != NULL) {
        char *token = strtok(line, " ");
        while(token){
            adj[currEdge] = (size_t) strtoul(token, NULL, 10) - 1;
            token = strtok(NULL, " ");
            adjw[currEdge++] = (double) strtold(token, NULL); 
            token = strtok(NULL, " ");
        };
        
        vtxw[currVtx++] = (size_t) 1;
        rowstart[currVtx] = currEdge;
    }
    free(line);
    return 0;
}
int readNodeWeighted(FILE* fh, std::vector<size_t>& rowstart, std::vector<size_t>& adj, std::vector<double>& adjw, std::vector<size_t>& vtxw){
    const int len = 1024;
    char* line = (char*) malloc(sizeof(char)*len);

    size_t currVtx = 0;
    size_t currEdge = 0;

    rowstart[0] = 0;
    while (fgets(line, len, fh) != NULL) {
        char *token = strtok(line, " ");
        vtxw[currVtx++] = (size_t) strtoul(token, NULL, 10);
        while((token = strtok(NULL, " "))){
            adj[currEdge] = (size_t) strtoul(token, NULL, 10) - 1;
            adjw[currEdge++] = (double) 1.0;
        };
        
        rowstart[currVtx] = currEdge;
    }
    free(line);
    return 0;
}
int readEdgeNodeWeighted(FILE* fh, std::vector<size_t>& rowstart, std::vector<size_t>& adj, std::vector<double>& adjw, std::vector<size_t>& vtxw){
    const int len = 1024;
    char* line = (char*) malloc(sizeof(char)*len);

    size_t currVtx = 0;
    size_t currEdge = 0;

    rowstart[0] = 0;
    while (fgets(line, len, fh) != NULL) {
        char *token = strtok(line, " ");
        vtxw[currVtx++] = (size_t) strtoul(token, NULL, 10);
        while((token = strtok(NULL, " "))){
            adj[currEdge] = (size_t) strtoul(token, NULL, 10) - 1;
            token = strtok(NULL, " ");
            adjw[currEdge++] = (double) strtold(token, NULL); 
        };
        
        rowstart[currVtx] = currEdge;
    }
    free(line);
    return 0;
}


int readFile(const char *fname, size_t* nVtx, size_t* nEdges, std::vector<size_t>& rowstart, std::vector<size_t>& adj, std::vector<size_t>& vtxw, std::vector<double>& adjw){

    FILE* fh = fopen(fname, "r");
    if (fh == NULL) return -1;

    *nVtx = -1, *nEdges = -1;
    int weightType = -1;

    char header[64];
    fgets(header, 64, fh);
    sscanf(header,"%ld %ld %d", nVtx, nEdges, &weightType);

    *nEdges *= 2; // Edges are put twice

    rowstart.reserve((*nVtx)+1);
    adj.reserve(*nEdges);
    vtxw.reserve(*nVtx);
    adjw.reserve(*nEdges);

    switch(weightType){

        case UNWEIGHTED:
            printf("Type of graph is UNWEIGHTED\n");
            readUnweighted(fh, rowstart, adj, adjw, vtxw);
            break;

        case EDGE_WEIGHTED:
            printf("Type of graph is EDGE_WEIGHTED\n");
            readEdgeWeighted(fh, rowstart, adj, adjw, vtxw);
            break;

        case NODE_WEIGHTED:
            printf("Type of graph is NODE_WEIGHTED\n");
            readNodeWeighted(fh, rowstart, adj, adjw, vtxw);
            break;

        case EDGE_NODE_WEIGHTED:
            printf("Type of graph is EDGE_NODE_WEIGHTED\n");
            readEdgeNodeWeighted(fh, rowstart, adj, adjw, vtxw);
            break;
        default:
            printf("Wrong type of graph\n");
    }
    
    return 0;
}

