#version 330 core

in vec4 vertexColor;
in vec3 fragPos;
in vec3 normal;
in vec3 viewDir;
in vec3 lightDir;
in vec3 h;

uniform float ka, kd, ks, p;

uniform vec3 lightColor;

out vec4 FragColor;

void main() {

    // All lighting components
    vec3 ambient = ka * lightColor;
    vec3 diffuse = kd * max(0.0, dot(normal, lightDir)) * lightColor;
    vec3 specular = ks * pow(max(0.0, dot(normal, h)), p) * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * vertexColor.rgb, 1.0);
}
