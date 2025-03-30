#version 330 core
layout (location = 0) in vec2 aPos;  // Pozycja 2D punktu
uniform vec2 center;
uniform float zoom;
uniform float pointSize; // Rozmiar punktu

void main() {
    vec2 pos = (aPos - center) * zoom;
    gl_Position = vec4(pos, 0.0, 1.0);
    gl_PointSize = pointSize;  // Ustawianie rozmiaru punktu
}
