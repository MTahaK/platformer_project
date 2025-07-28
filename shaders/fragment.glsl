#version 330 core

uniform vec4 color;
out vec4 FragColor; // Output color
void main()
{
	// Set the fragment color to the uniform color
	FragColor = color;
}