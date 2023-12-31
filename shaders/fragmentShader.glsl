#version 460 core
in float radius;
in vec2 pos;
in vec4 vertexColor;

out vec4 FragColor;

void main() {

    float dotprod = dot(pos, pos);
    if (dotprod >= radius*radius) 
        discard;

    if ((0.75*radius)*(0.75*radius) < dotprod)
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else 
        FragColor = vertexColor;
} 
