#version 330 core

in vec4 vertexColor;
in vec3 fragPos;
in vec3 normal;

uniform vec3 viewPos;
uniform float ka, kd, ks, p;

uniform vec3 lightPos;
uniform vec3 lightColor;

out vec4 FragColor;

void main() {
    // Only ambient lighting - no expensive calculations
    vec3 ambient = ka * lightColor;

    FragColor = vec4(ambient * vertexColor.rgb, 1.0);
}
