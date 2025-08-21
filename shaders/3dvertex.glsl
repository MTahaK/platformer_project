#version 330 core

layout(location = 0) in vec3 aPos; // Vertex position, 3D coordinates
layout(location = 1) in vec4 aColor; // Vertex color
layout(location = 2) in vec3 aNormal; // Vertex normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec4 color;

out vec4 vertexColor; // Output color to fragment shader

void main() {
	// Transform the vertex position using the MVP matrix
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vertexColor = color; // Pass the uniform color to the fragment shader
}