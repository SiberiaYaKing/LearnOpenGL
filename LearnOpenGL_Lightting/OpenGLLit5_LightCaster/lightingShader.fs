
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 viewPos;

struct DirectionalLight {
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight directionalLight;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

void main(){
	vec3 ambient = directionalLight.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*directionalLight.diffuse;

	vec3 viewDir = normalize(viewPos-FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	vec3 specular = vec3(texture(material.specular,TexCoords))*spec*directionalLight.specular;
	vec3 result = (ambient+diffuse+specular).rgb;
	FragColor = vec4(result,1.0);
}

