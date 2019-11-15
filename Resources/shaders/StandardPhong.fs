
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 viewPos;

struct DirLight {
	bool isOn;
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
	bool isOn;
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
	bool isOn;
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

#define MAX_TEXTURE_COUNT 4

uniform	sampler2D texture_diffuse1;
uniform	sampler2D texture_specular1;
uniform	bool has_texture=true;
uniform	float shininess;



vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir);

void main(){
	//Mix ALL Light
	vec3 viewDir = normalize(viewPos-FragPos);
	vec3 norm = normalize(Normal);

	vec3 result = CalcDirLight(dirLight,norm,viewDir);
	for(int i=0;i<NR_POINT_LIGHTS;i++)
		result += CalcPointLight(pointLights[i],norm,FragPos,viewDir);
	result += CalcSpotLight(spotLight,norm,FragPos,viewDir);
		
	FragColor = vec4(result,1.0);
}

vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir){
	if(light.isOn){
		vec3 lightDir = normalize(-light.direction);
		float diff= max(dot(lightDir,normal),0);
		vec3 reflectDir = reflect(-lightDir,normal);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);

		vec3 ambient,diffuse,specular;
		if(has_texture){
			ambient =light.ambient*vec3(texture(texture_diffuse1,TexCoords));
				diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,TexCoords));
			specular = light.specular*spec*vec3(texture(texture_specular1,TexCoords));
		}
		else{
			ambient = light.ambient*vec3(0.05,0.05,0.05);
			diffuse = light.diffuse*diff*vec3(0.5,0.5,0.5);
			specular = light.specular*spec*vec3(0.7,0.7,0.7);
		}
		return (ambient+diffuse+specular);
	}
	else return vec3(0);
}

vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir){
	if(light.isOn){
		vec3 lightDir = normalize(light.position-fragPos);
		float diff= max(dot(lightDir,normal),0);
		vec3 reflectDir = reflect(-lightDir,normal);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);

		float dis = distance(light.position,fragPos);
		float attenuation = 1.0/(light.constant+light.linear*dis+light.quadratic*dis*dis);

		vec3 ambient,diffuse,specular;
		if(has_texture){
			ambient =light.ambient*vec3(texture(texture_diffuse1,TexCoords));
				diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,TexCoords));
			specular = light.specular*spec*vec3(texture(texture_specular1,TexCoords));
		}
		else{
			ambient = light.ambient*vec3(0.05,0.05,0.05);
			diffuse = light.diffuse*diff*vec3(0.5,0.5,0.5);
			specular = light.specular*spec*vec3(0.7,0.7,0.7);
		}
		ambient *= attenuation;
		specular*= attenuation;
		diffuse *= attenuation;
		return (ambient+diffuse+specular);
	}
	else return vec3(0);
}

vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir){
	vec3 lightDir = normalize(light.position-fragPos);
	float cos_theta = dot(-lightDir,normalize(light.direction));
	float cos_phi = light.cutOff;
	float cos_gamma = light.outterCutOff;
	float intensity = clamp((cos_theta-cos_gamma)/(cos_phi-cos_gamma),0,1);

	vec3 ambient,diffuse,specular;

	if(light.isOn){
		if(cos_theta>cos_gamma){
			float diff = max(dot(lightDir,normal),0);
			vec3 reflectDir = reflect(-lightDir,normal);
			float spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);

			if(has_texture){
				ambient =light.ambient*vec3(texture(texture_diffuse1,TexCoords));
				 diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,TexCoords));
				specular = light.specular*spec*vec3(texture(texture_specular1,TexCoords));
			}
			else{
				ambient = light.ambient*vec3(0.05,0.05,0.05);
				diffuse = light.diffuse*diff*vec3(0.5,0.5,0.5);
				specular = light.specular*spec*vec3(0.7,0.7,0.7);
			}

			float dis = distance(light.position,fragPos);
			float attenuation = 1.0/(light.constant+light.linear*dis+light.quadratic*dis*dis);

			diffuse*=(attenuation*intensity);
			specular*=(attenuation*intensity);
			return (diffuse+specular+ambient);
		}
		else{
			if(has_texture)
				return light.ambient*vec3(texture(texture_diffuse1,TexCoords));
			else
				return light.ambient*vec3(0.05,0.05,0.05);
		}
	}
	else return vec3(0);
}



