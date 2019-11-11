
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

	float dis = distance(light.position,fragPos);
	float attenuation = 1.0/(light.constant+light.linear*dis+light.quadratic*dis*dis);

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


		float dis = distance(light.position,fragPos);
		float attenuation = 1.0/(light.constant+light.linear*dis+light.quadratic*dis*dis);

		diffuse*=(attenuation*intensity);
		specular*=(attenuation*intensity);
		return (diffuse+specular+ambient);
	}
	else{
		return ambient;
	}
}