
#version 330 core

#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


out vec4 FragColor;

uniform Material material;
uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;
uniform vec3 u_lightSourcePos;
uniform vec3 u_ViewPos;

in vec3 FragPos;
in vec3 Normal;

void main() {

    // ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * u_LightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_lightSourcePos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * u_LightColor;

    vec3 result = (ambient + diffuse + specular) * u_ObjectColor;
    FragColor = vec4(result, 1.0f);
}