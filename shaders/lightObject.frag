
#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


out vec4 FragColor;

uniform Light u_Light;
uniform Material u_Material;

uniform vec3 u_lightSourcePos;
uniform vec3 u_ViewPos;

in vec3 FragPos;
in vec3 Normal;

void main() {

    // ambient
    vec3 ambient = u_Light.ambient * u_Material.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_lightSourcePos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * (diff * u_Material.diffuse);

    // specular
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_Material.shininess);
    vec3 specular =  (spec * u_Material.specular) * u_Light.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}