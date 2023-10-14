#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 color;
uniform mat2 rotation;
uniform vec2 translation;
out vec4 vertexColor;

void main(){
    gl_Position = vec4(rotation * aPos + translation, 0.0, 1.0);
    vertexColor = vec4(color, 1.0);
}
