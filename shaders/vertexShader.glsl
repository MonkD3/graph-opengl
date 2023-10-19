#version 460 core
layout (location = 0) in vec2 vPos; // Vertex position
layout (location = 1) in vec2 cPos; // Center position (circle)
layout (location = 2) in vec3 color;
uniform mat2 rotation;
uniform vec2 translation;
out float radius;
out vec4 vertexColor;
out vec2 pos;

void main(){
    radius = 0.5f; // constant for now, can be in the layout later
    gl_Position = vec4(rotation * (radius* vPos + cPos) + translation, 0.0, 1.0);
    vertexColor = vec4(color, 1.0);
    pos = vPos;
}
