
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 viewPos;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material{
	sampler2D emission;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

void main(){
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position-FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*light.diffuse;

	vec3 viewDir = normalize(viewPos-FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	vec3 specular = vec3(texture(material.specular,TexCoords))*spec*light.specular;

	vec3 emission = texture(material.emission,TexCoords).rgb;

	vec3 result = (ambient+diffuse+specular+emission).rgb;
	FragColor = vec4(result,1.0);
}

