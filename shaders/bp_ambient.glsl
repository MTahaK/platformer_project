#version 330 core

in vec4 vertexColor;
in vec3 fragPos;
in vec3 normal;

uniform vec3 viewPos;
uniform float ka, kd, ks, p;

const vec3 lightPos = vec3(100.0, 100.0, -10.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

out vec4 FragColor;

void main() {
    // Only ambient lighting - no expensive calculations
    vec3 ambient = ka * lightColor;

    FragColor = vec4(ambient * vertexColor.rgb, 1.0);
}
