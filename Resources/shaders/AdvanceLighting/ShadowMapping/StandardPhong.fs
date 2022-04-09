
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
	vec2 TexCoords;
    vec3 Normal;
    vec4 FragPosLightSpace;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	//mat3 TBN; //debug
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

uniform	sampler2D texture_diffuse1;
uniform	sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D shadowMap;
uniform	bool has_texture=true;
uniform	float shininess;
uniform bool useNormalMap;


float CalcShadow(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal);
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);

const float gamma = 2.2f;
const float invGamma = 0.454546f; // 1.0/gamma
vec4 DecodeGamma(vec4 src){
    return pow(src,vec4(invGamma));
}

void main(){
	vec3 norm,viewDir;
	if(useNormalMap){
		viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
		//viewDir = normalize(viewPos-fs_in.FragPos);//debug
		norm = texture(texture_normal1, fs_in.TexCoords).rgb;
		norm = normalize(norm * 2.0 - 1.0);
		//norm = normalize(transpose(fs_in.TBN)*norm); //debug
	}
	else{
		viewDir = normalize(viewPos-fs_in.FragPos);
		norm = normalize(fs_in.Normal);
	}

	vec3 result = CalcDirLight(dirLight,norm,viewDir);
	FragColor = DecodeGamma(vec4(result,1.0));
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
		vec3 lightDir;
		if(useNormalMap){
			lightDir = normalize(-fs_in.TangentLightDir);
		}
		else{
			lightDir = normalize(-light.direction);
		}
		
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

		float shadowFac = CalcShadow(fs_in.FragPosLightSpace,normalize(-light.direction),fs_in.Normal);
		return (ambient+shadowFac*(diffuse+specular));
	}
	else return vec3(0);
}



