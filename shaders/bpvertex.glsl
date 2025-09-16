#version 330 core

layout(location = 0) in vec3 aPos; // Vertex position, 3D coordinates
layout(location = 1) in vec4 aColor; // Vertex color
layout(location = 2) in vec3 aNormal; // Vertex normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMatrix;

uniform vec3 viewPos;
uniform vec3 lightPos;

out vec4 vertexColor; // Output color to fragment shader
out vec3 normal;
out vec3 viewDir;
out vec3 lightDir;
out vec3 h;

void main() {
	// Transform the vertex position using the MVP matrix
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	vertexColor = aColor; // Pass the vertex color to the fragment shader
	vec3 fragPos = vec3(model * vec4(aPos, 1.0));
	normal = normalize(normalMatrix * aNormal);
	viewDir = normalize(viewPos - fragPos);
	lightDir = normalize(lightPos - fragPos);
	h = normalize(lightDir + viewDir);
}