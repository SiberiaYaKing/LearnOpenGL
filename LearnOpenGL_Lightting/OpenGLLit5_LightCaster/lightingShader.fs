
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

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight;

struct SpotLight{
	vec3 position;
	vec3 direction;

	float cutOff;  
	float outterCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform SpotLight spotLight;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

void main(){
//	//DirectionalLight
//	vec3 ambient = directionalLight.ambient * texture(material.diffuse, TexCoords).rgb;
//
//	vec3 norm = normalize(Normal);
//	vec3 lightDir = normalize(-directionalLight.direction);
//	float diff = max(dot(norm,lightDir),0.0);
//	vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*directionalLight.diffuse;
//
//	vec3 viewDir = normalize(viewPos-FragPos);
//	vec3 reflectDir = reflect(-lightDir,norm);
//	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
//	vec3 specular = vec3(texture(material.specular,TexCoords))*spec*directionalLight.specular;

//	//Pointlight
//	vec3 ambient = pointLight.ambient * texture(material.diffuse, TexCoords).rgb;
//
//	vec3 norm = normalize(Normal);
//	vec3 lightDir = normalize(pointLight.position-FragPos);
//	float diff = max(dot(norm,lightDir),0.0);
//	vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*pointLight.diffuse;
//
//	vec3 viewDir = normalize(viewPos-FragPos);
//	vec3 reflectDir = reflect(-lightDir,norm);
//	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
//	vec3 specular = vec3(texture(material.specular,TexCoords))*spec*pointLight.specular;
//
//	float distance = length(pointLight.position -FragPos);
//	float attenuation = 1.0/(pointLight.constant+pointLight.linear*distance+pointLight.quadratic*(distance*distance));
//	ambient*=attenuation;
//	diffuse*=attenuation;
//	specular*=attenuation;

	//SpotLight
	vec3 lightDir = normalize(spotLight.position-FragPos);
	float cos_theta = dot(lightDir,normalize(-spotLight.direction)); //cause camera.front is (相反)opposite
	float cos_phi = spotLight.cutOff;
	float cos_gamma = spotLight.outterCutOff;
	float epsilon = cos_phi-cos_gamma;
	float intensity = clamp((cos_theta-cos_gamma)/epsilon,0.0,1.0);

	if(cos_theta>cos_gamma){
		vec3 ambient = spotLight.ambient * texture(material.diffuse, TexCoords).rgb;

		vec3 norm = normalize(Normal);
		float diff = max(dot(norm,lightDir),0.0);
		vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*spotLight.diffuse;

		vec3 viewDir = normalize(viewPos-FragPos);
		vec3 reflectDir = reflect(-lightDir,norm);
		float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
		vec3 specular = vec3(texture(material.specular,TexCoords))*spec*spotLight.specular;

		float distance = length(spotLight.position -FragPos);
		float attenuation = 1.0/(spotLight.constant+spotLight.linear*distance+spotLight.quadratic*(distance*distance));
		//不对环境光做出影响,让它总是能有一点光 Do not affect the ambient light, so that it can always have a little light
		//ambient*=attenuation; 
		diffuse*=attenuation;
		specular*=attenuation;
		//ambient *=intensity; 
		diffuse  *= intensity;
		specular *= intensity;

		vec3 result = (ambient+diffuse+specular).rgb;
		FragColor = vec4(result,1.0);
	}
	else  {
		FragColor = vec4(spotLight.ambient * texture(material.diffuse, TexCoords).rgb,1);
	}
}