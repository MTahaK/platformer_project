#version 330 core

layout(location = 0) in vec2 aPos; // Vertex position, 2D coordinates
layout(location = 1) in vec2 aTexCoord; // Texture coordinates

uniform mat4 MVP; // Model-View-Projection matrix
uniform vec2 u_UVScale;
uniform vec2 u_UVOffset;

out vec2 v_TexCoord; // Pass UV coordinates to fragment shader

void main()
{
	// Transform the vertex position using the MVP matrix
	gl_Position = MVP * vec4(aPos, 0.0, 1.0);
	// v_TexCoord = aTexCoord;
	v_TexCoord = aTexCoord * u_UVScale + u_UVOffset;
}