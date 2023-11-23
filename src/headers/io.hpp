#ifndef __IO_HPP
#define __IO_HPP

#include <stddef.h>
#include <stdio.h>
#include <vector>

typedef enum {
    UNWEIGHTED = -1,
    EDGE_WEIGHTED = 1,
    NODE_WEIGHTED = 10,
    EDGE_NODE_WEIGHTED = 11
} graphWeightType;

int readFile(
    const char *fname, 
    size_t* nVtx, 
    size_t* nEdges, 
    std::vector<size_t>& rowstart, 
    std::vector<size_t>& adj, 
    std::vector<size_t>& vtxw,
    std::vector<double>& adjw
);
int readUnweighted(     
    FILE* fh, 
    std::vector<size_t>& rowstart, 
    std::vector<size_t>& adj, 
    std::vector<double>& adjw, 
    std::vector<size_t>& vtxw
);
int readEdgeWeighted(    
    FILE* fh, 
    std::vector<size_t>& rowstart, 
    std::vector<size_t>& adj, 
    std::vector<double>& adjw, 
    std::vector<size_t>& vtxw
);
int readNodeWeighted(    
    FILE* fh, 
    std::vector<size_t>& rowstart, 
    std::vector<size_t>& adj, 
    std::vector<double>& adjw, 
    std::vector<size_t>& vtxw
);
int readEdgeNodeWeighted(
    FILE* fh, 
    std::vector<size_t>& rowstart, 
    std::vector<size_t>& adj, 
    std::vector<double>& adjw, 
    std::vector<size_t>& vtxw
);

#endif // __IO_HPP
