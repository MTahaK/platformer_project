#version 330 core

in vec4 vertexColor;  // Receive interpolated color from vertex shader

out vec4 FragColor;   // Final pixel color

void main() {
    FragColor = vertexColor;  // Use the interpolated vertex color
}