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
    vec3 nNormal = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 h = normalize(lightDir + viewDir);

    // All lighting components
    vec3 ambient = ka * lightColor;
    vec3 diffuse = kd * max(0.0, dot(nNormal, lightDir)) * lightColor;
    vec3 specular = ks * pow(max(0.0, dot(nNormal, h)), p) * lightColor;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * vertexColor.rgb, 1.0);
}
