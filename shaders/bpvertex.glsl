#version 330 core

layout(location = 0) in vec3 aPos; // Vertex position, 3D coordinates
layout(location = 1) in vec4 aColor; // Vertex color
layout(location = 2) in vec3 aNormal; // Vertex normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 vertexColor; // Output color to fragment shader
out vec3 fragPos;
out vec3 normal;

void main() {
	// Transform the vertex position using the MVP matrix
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vertexColor = aColor; // Pass the vertex color to the fragment shader
	fragPos = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * aNormal;
}