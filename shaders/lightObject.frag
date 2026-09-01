
#version 330 core

struct Material {
    sampler2D diffuse;

    sampler2D specular;
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

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main() {

    // ambient
    vec3 ambient = u_Light.ambient * vec3(texture(u_Material.diffuse, TexCoords));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_lightSourcePos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * diff * vec3(texture(u_Material.diffuse, TexCoords));

    // specular
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_Material.shininess);
    vec3 specular = u_Light.specular * spec * vec3(texture(u_Material.specular, TexCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}