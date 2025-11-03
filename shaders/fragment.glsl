#version 330 core

in vec2 v_TexCoord; // UV coordinates from vertex shader

uniform vec4 color;
uniform int useTexture;
uniform sampler2D slot;

out vec4 FragColor; // Output color
void main()
{
	if(useTexture == 1){
		// vec2 uv = v_TexCoord * u_UVScale + u_UVOffset;
		FragColor = texture(slot, v_TexCoord);
	} else {
		FragColor = color;
	}
}