#version 330 core
out vec4 FragColor;
in vec4 color; // Interpolated color from vertex shader

void main() {
   
        FragColor = vec4(0.0, 1.0, 1.0, 1.0); 
}
