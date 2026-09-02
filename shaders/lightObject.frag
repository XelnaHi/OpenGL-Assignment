#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material u_Material;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight u_DirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLight u_PointLights[NR_POINT_LIGHTS];
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
uniform SpotLight u_SpotLight;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_ViewPos;

out vec4 FragColor;

void main() {

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);

    // apply Directional light
    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    // apply Point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(u_PointLights[i], norm, FragPos, viewDir);
    }

    // apply Spotlight
    result += CalcSpotLight(u_SpotLight, norm, FragPos, viewDir);

    // return result
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    // emission
    vec3 emission = texture(u_Material.emission, TexCoords).rgb;

    // combine
    vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, TexCoords));

    //    return (ambient + diffuse + specular + emission);
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // emission
    vec3 emission = texture(u_Material.emission, TexCoords).rgb;

    // combine
    vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    emission *= attenuation;

    //    return vec3(ambient + diffuse + specular + emission);
    return (ambient + diffuse + specular);

}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir){

    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_Material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // emission
    vec3 emission = texture(u_Material.emission, TexCoords).rgb;

    // intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine
    vec3 ambient = light.ambient * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(u_Material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(u_Material.specular, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    emission *= attenuation * intensity;

//    return ambient + diffuse + specular + emission;
    return (ambient + diffuse + specular );

}