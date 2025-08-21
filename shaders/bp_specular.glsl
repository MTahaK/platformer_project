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

    // Only specular lighting - includes expensive pow() function
    vec3 specular = ks * pow(max(0.0, dot(nNormal, h)), p) * lightColor;
  	
    // diffuse 
    // vec3 norm = normalize(normal);
    // vec3 lightDir = normalize(lightPos - fragPos);
    
    // // specular
    // float specularStrength = 0.5;
    // vec3 viewDir = normalize(viewPos - fragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);  
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    // vec3 specular = specularStrength * spec * lightColor;  

    FragColor = vec4(specular * vertexColor.rgb, 1.0);
}
