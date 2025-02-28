#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoords; // Pass to fragment shader

void main() {
    TexCoords = (aPos.xy + vec2(1.0, 1.0)) * 0.5; 
    gl_Position = vec4(aPos, 1.0);
}
