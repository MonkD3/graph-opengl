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
