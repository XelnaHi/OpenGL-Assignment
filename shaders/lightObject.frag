#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
struct Light {
    vec3 position; // used for point lights (light fades over a distance)
//  vec3 direction; // used for directional lights (light source from infinity far, making all light rays parallell)

// spotlight. also used position as part of it's calculations
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


out vec4 FragColor;

uniform Light u_Light;
uniform Material u_Material;

uniform vec3 u_ViewPos;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main() {

    //    vec3 lightDir = normalize(-u_Light.direction); // directional light
    vec3 lightDir = normalize(u_Light.position - FragPos);

    float theta = dot(lightDir, normalize(-u_Light.direction));
    float epsilon = u_Light.cutOff - u_Light.outerCutOff;
    float intensity = clamp((theta - u_Light.outerCutOff) / epsilon, 0.0, 1.0);

    // ambient
    vec3 ambient = u_Light.ambient * texture(u_Material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * diff * texture(u_Material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), u_Material.shininess);
    vec3 specular = u_Light.specular * spec * texture(u_Material.specular, TexCoords).rgb;

    // emission
    vec3 emission = texture(u_Material.emission, TexCoords).rgb;

    // attenuation
    float distance = length(u_Light.position - FragPos);
    float attenuation = 1.0 / (u_Light.constant + u_Light.linear * distance + u_Light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    emission *= attenuation;

    diffuse *= intensity;
    specular *= intensity;
    emission *= intensity;

    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0f);
}