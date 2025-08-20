#version 330 core

in vec4 vertexColor;  // Receive interpolated color from vertex shader
in vec3 fragPos;
in vec3 normal;      // Normalized

uniform vec3 viewPos;

// Hard-coded light properties
const vec3 lightPos = vec3(100.0, 100.0, 100.0);      
const vec3 lightColor = vec3(1.0, 1.0, 1.0);    // White light

out vec4 FragColor;   // Final pixel color

void main() {

    vec3 nNormal = normalize(normal); // Normalize the normal vector
    vec3 viewDir = normalize(viewPos - fragPos); // Direction FROM fragment TO camera 
    vec3 lightDir = normalize(lightPos - fragPos); // Direction FROM fragment TO light

    float ka = 0.1;      // Ambient coefficient
    float kd = 0.8;      // Diffuse coefficient
    float ks = 1.0;      // Specular coefficient
    float p = 32.0;      // Shininess exponent

    vec3 h = normalize(lightDir + viewDir);

    vec3 ambient = ka * lightColor;
    vec3 diffuse = kd * max(0, dot(nNormal, lightDir)) * lightColor;
    vec3 specular = ks * pow( max(0, dot(nNormal, h)), p) * lightColor;

    vec3 l = ambient + diffuse + specular;

    FragColor = vec4(l * vertexColor.rgb, 1.0);  // Use the computed lighting color
}