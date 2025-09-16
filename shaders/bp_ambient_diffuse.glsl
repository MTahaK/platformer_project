// #version 330 core

// in vec4 vertexColor;
// in vec3 fragPos;
// in vec3 normal;

// uniform vec3 viewPos;
// uniform float ka, kd, ks, p;

// uniform vec3 lightPos;
// uniform vec3 lightColor;

// out vec4 FragColor;

// void main() {
//     vec3 nNormal = normalize(normal);
//     vec3 lightDir = normalize(lightPos - fragPos);

//     // Ambient + Diffuse - no expensive pow() function
//     vec3 ambient = ka * lightColor;
//     vec3 diffuse = kd * max(0.0, dot(nNormal, lightDir)) * lightColor;

//     vec3 result = ambient + diffuse;
//     FragColor = vec4(result * vertexColor.rgb, 1.0);
// }

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

    vec3 result = ambient + diffuse;
    FragColor = vec4(result * vertexColor.rgb, 1.0);
}
