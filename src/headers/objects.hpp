#include <cmath>
#include <math.h>
class Vertex {
    public:
        float x = 0.0f; // X position 
        float y = 0.0f; // Y position
        float c[3] = {1.0f, 1.0f, 1.0f}; // color

        // Constructors
        Vertex(){};
        Vertex(float _x, float _y): x(_x), y(_y){};
        Vertex(float _x, float _y, float* _c): x(_x), y(_y) {
            c[0] = _c[0];
            c[1] = _c[1];
            c[2] = _c[2];
        };

        // Destructor
        ~Vertex(){};
};

struct Disk {
    float x = 0.0f;
    float y = 0.0f;
    float r = 0.1f;
    float c[3] = {1.0f, 1.0f, 1.0f}; // color
    int ndots = 10;
    Vertex* v;

    Disk(){};
    Disk(float _x, float _y): x(_x), y(_y){}; 
    Disk(float _x, float _y, float _r): x(_x), y(_y), r(_r){}; 
    Disk(float _x, float _y, float _r, int _n): x(_x), y(_y), r(_r), ndots(_n){}; 

    void setColor(float r, float g, float b){
        c[0] = r, c[1] = g, c[2] = b;
    };

    void draw(){
        
        for (int i = 0; i < ndots; i++){
            float px = r*cosf(i*2.f*M_PI/ndots);
            float py = r*sinf(i*2.f*M_PI/ndots);
            v[i].x = x + px; 
            v[i].y = y + py; 
        }
    }
};
