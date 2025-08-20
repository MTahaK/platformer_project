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
    vec3 nNormal = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Ambient + Diffuse - no expensive pow() function
    vec3 ambient = ka * lightColor;
    vec3 diffuse = kd * max(0.0, dot(nNormal, lightDir)) * lightColor;

    vec3 result = ambient + diffuse;
    FragColor = vec4(result * vertexColor.rgb, 1.0);
}
