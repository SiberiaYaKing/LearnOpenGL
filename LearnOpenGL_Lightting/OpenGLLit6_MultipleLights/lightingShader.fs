
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 viewPos;

struct DirLight {
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];


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
	sampler2D film;
	float shininess;
};
uniform Material material;

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,Material mat);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir,Material mat);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir,Material mat);

void main(){
	//SpotLight
//	vec3 lightDir = normalize(spotLight.position-FragPos);
//	float cos_theta = dot(lightDir,normalize(-spotLight.direction)); //cause camera.front is (相反)opposite
//	float cos_phi = spotLight.cutOff;
//	float cos_gamma = spotLight.outterCutOff;
//	float epsilon = cos_phi-cos_gamma;
//	float intensity = clamp((cos_theta-cos_gamma)/epsilon,0.0,1.0);
//
//	if(cos_theta>cos_gamma){
//		vec3 ambient = spotLight.ambient * texture(material.diffuse, TexCoords).rgb;
//
//		vec3 norm = normalize(Normal);
//		float diff = max(dot(norm,lightDir),0.0);
//		vec3 diffuse = vec3(texture(material.diffuse,TexCoords))*diff*spotLight.diffuse;
//
//		vec3 viewDir = normalize(viewPos-FragPos);
//		vec3 reflectDir = reflect(-lightDir,norm);
//		float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
//		vec3 specular = vec3(texture(material.specular,TexCoords))*spec*spotLight.specular;
//
//		float distance = length(spotLight.position -FragPos);
//		float attenuation = 1.0/(spotLight.constant+spotLight.linear*distance+spotLight.quadratic*(distance*distance));
//		//不对环境光做出影响,让它总是能有一点光 Do not affect the ambient light, so that it can always have a little light
//		//ambient*=attenuation; 
//		diffuse*=attenuation;
//		specular*=attenuation;
//		//ambient *=intensity; 
//		diffuse  *= intensity;
//		specular *= intensity;
//
//		vec3 result = (ambient+diffuse+specular).rgb;
//		FragColor = vec4(result,1.0);
//	}
//	else  {
//		FragColor = vec4(spotLight.ambient * texture(material.diffuse, TexCoords).rgb,1);
//	}


	//Mix ALL Light
	vec3 viewDir = normalize(viewPos-FragPos);
	vec3 norm = normalize(Normal);

	vec3 result = CalcDirLight(dirLight,norm,viewDir,material);
	for(int i=0;i<NR_POINT_LIGHTS;i++)
		result += CalcPointLight(pointLights[i],norm,FragPos,viewDir,material);
	result += CalcSpotLight(spotLight,norm,FragPos,viewDir,material);
		
	FragColor = vec4(result,1.0);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir,Material mat){
	vec3 lightDir = normalize(-light.direction);//The direction of the light we use is always the opposite
	float diff= max(dot(lightDir,normal),0);
	vec3 reflectDir = reflect(-lightDir,normal);//the direction of lightDir and reflectDir is down ,just like that "\|/"
	float spec = pow(max(dot(reflectDir,viewDir),0.0),mat.shininess);

	vec3 ambient =light.ambient*vec3(texture(mat.diffuse,TexCoords));
	vec3 diffuse = light.diffuse*diff*vec3(texture(mat.diffuse,TexCoords));
	vec3 specular = light.specular*spec*vec3(texture(mat.specular,TexCoords));
	return (ambient+diffuse+specular);
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir,Material mat){
	vec3 lightDir = normalize(light.position-fragPos);
	float diff = max(dot(lightDir,normal),0);
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(reflectDir,viewDir),0.0),mat.shininess);

	float distance = length(light.position-fragPos);
	float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);

	vec3 ambient = light.ambient*texture(mat.diffuse,TexCoords).rgb;
	vec3 diffuse = light.diffuse*diff*texture(material.diffuse,TexCoords).rgb;
	vec3 specular = light.specular*spec*texture(material.specular,TexCoords).rgb;
	ambient *= attenuation;
	specular*= attenuation;
	diffuse *= attenuation;
	return (ambient+diffuse+specular);
}

vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir,Material mat){
	vec3 lightDir = normalize(light.position-fragPos);
	float cos_theta = dot(-lightDir,normalize(light.direction));
	float cos_phi = light.cutOff;
	float cos_gamma = light.outterCutOff;
	float intensity = clamp((cos_theta-cos_gamma)/(cos_phi-cos_gamma),0,1);

	vec3 ambient= light.ambient*texture(material.diffuse,TexCoords).rgb;

	if(cos_theta>cos_gamma){
		float diff = max(dot(lightDir,normal),0);
		vec3 reflectDir = reflect(-lightDir,normal);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shininess);

		vec3 diffuse = light.diffuse*diff*texture(material.diffuse,TexCoords).rgb;
		//vec3 diffuse = light.diffuse*diff* mix(texture(material.diffuse,TexCoords),texture(material.film,TexCoords),0.8).rgb;
		vec3 specular = light.specular*spec*texture(material.specular,TexCoords).rgb;


		float distance = length(light.position-fragPos);
		float attenuation = 1.0/(light.constant+light.linear*distance+light.quadratic*distance*distance);

		diffuse*=(attenuation*intensity);
		specular*=(attenuation*intensity);
		return (diffuse+specular+ambient);
	}
	else{
		return ambient;
	}
}