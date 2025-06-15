#version 330 core

layout(location = 0) in vec2 aPos; // Vertex position, 2D coordinates

uniform mat4 MVP; // Model-View-Projection matrix

void main()
{
    // Transform the vertex position using the MVP matrix
    gl_Position = MVP * vec4(aPos, 0.0, 1.0);
}