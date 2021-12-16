
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

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


uniform	sampler2D texture_diffuse1;
uniform	sampler2D texture_specular1;
uniform sampler2D shadowMap;
uniform	bool has_texture=true;
uniform	float shininess;


float CalcShadow(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal);
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir);
vec3 CalcSpotLight(SpotLight light,vec3 normal,vec3 fragPos,vec3 viewDir);

void main(){
	//Mix ALL Light
	vec3 viewDir = normalize(viewPos-fs_in.FragPos);
	vec3 norm = normalize(fs_in.Normal);

	vec3 result = CalcDirLight(dirLight,norm,viewDir);
	for(int i=0;i<NR_POINT_LIGHTS;i++)
		result += CalcPointLight(pointLights[i],norm,fs_in.FragPos,viewDir);
	result += CalcSpotLight(spotLight,norm,fs_in.FragPos,viewDir);
	
	FragColor = vec4(result,1.0);
}

float CalcShadow(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal){
	//根据坡度变化的bias?
	float bias = max(-0.05 * (1.0 - dot(normal, lightDir)), -0.005);
	//执行透视除法
	vec3 projCoords = fragPosLightSpace.xyz/ fragPosLightSpace.w;
	projCoords = projCoords * 0.5+0.5;
	float currentDepth = projCoords.z;

	// with pcf
	float inLight = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			inLight += step(currentDepth+bias,pcfDepth);        
		}    
	}
	inLight /= 9.0;
	// // no pcf
	// float closestDepth = texture(shadowMap,projCoords.xy).r;
	// float inLight = step(currentDepth+bias,closestDepth);
	if(projCoords.z > 1.0) inLight = 1.0;
	return inLight;
}



vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir){
	if(light.isOn){
		vec3 lightDir = normalize(-light.direction);
		float diff= max(dot(lightDir,normal),0);
		vec3 reflectDir = reflect(-lightDir,normal);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),shininess);

		vec3 ambient,diffuse,specular;
		if(has_texture){
			ambient =light.ambient*vec3(texture(texture_diffuse1,fs_in.TexCoords));
			diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,fs_in.TexCoords));
			specular = light.specular*spec*vec3(texture(texture_specular1,fs_in.TexCoords));
		}
		else{
			ambient = light.ambient*vec3(0.05,0.05,0.05);
			diffuse = light.diffuse*diff*vec3(0.5,0.5,0.5);
			specular = light.specular*spec*vec3(0.7,0.7,0.7);
		}

		float shadowFac = CalcShadow(fs_in.FragPosLightSpace,lightDir,normal);
		return (ambient+shadowFac*(diffuse+specular));
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
			ambient =light.ambient*vec3(texture(texture_diffuse1,fs_in.TexCoords));
			diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,fs_in.TexCoords));
			specular = light.specular*spec*vec3(texture(texture_specular1,fs_in.TexCoords));
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
				ambient =light.ambient*vec3(texture(texture_diffuse1,fs_in.TexCoords));
				diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1,fs_in.TexCoords));
				specular = light.specular*spec*vec3(texture(texture_specular1,fs_in.TexCoords));
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
				return light.ambient*vec3(texture(texture_diffuse1,fs_in.TexCoords));
			else
				return light.ambient*vec3(0.05,0.05,0.05);
		}
	}
	else return vec3(0);
}



