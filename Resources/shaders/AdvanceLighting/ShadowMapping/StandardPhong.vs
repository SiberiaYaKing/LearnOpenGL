#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

struct DirLight {
	bool isOn;
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

uniform vec3 viewPos;
uniform bool useNormalMap;

//切线空间下的光照
out VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
	vec4 FragPosLightSpace;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
	//mat3 TBN; //debug
} vs_out;

void main()
{
	mat3 normalMat = transpose(inverse(mat3(model)));
	vs_out.FragPos = vec3(model*vec4(aPos,1.0));
	vs_out.Normal = normalize(normalMat*aNormal);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix*vec4(vs_out.FragPos,1.0);
	if(useNormalMap){
		vec3 tangent = normalize(normalMat*aTangent);
		tangent = normalize(tangent-dot(tangent,vs_out.Normal)*vs_out.Normal);
		vec3 bitangent = normalize(cross(tangent,vs_out.Normal));
		mat3 TBN = transpose(mat3(tangent,bitangent,vs_out.Normal));
		vs_out.TangentLightDir = TBN*dirLight.direction;
		vs_out.TangentFragPos = TBN*vs_out.FragPos;
		vs_out.TangentViewPos = TBN*viewPos;
		//vs_out.TBN = TBN; //debug
	}
}