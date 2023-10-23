#version 460 core
layout (location = 0) in vec2 vPos; // Vertex position
uniform mat2 rotation;
uniform vec2 translation;

void main(){
    gl_Position = vec4(rotation * vPos + translation, 0.0, 1.0);
}
