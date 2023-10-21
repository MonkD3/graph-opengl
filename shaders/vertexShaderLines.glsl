#version 460 core
layout (location = 0) in vec2 vPos; // Vertex position
uniform mat2 rotation;

void main(){
    gl_Position = vec4(rotation * vPos, 0.0, 1.0);
}
