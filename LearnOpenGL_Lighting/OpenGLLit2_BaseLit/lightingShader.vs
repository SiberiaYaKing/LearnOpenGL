//+++++++caculate the specular in world space++++++
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMat;

out vec3 Normal;
out vec3 FragPos;

void main()
{

	FragPos = vec3(model*vec4(aPos,1.0));
	Normal = normalMat*aNormal;
	//Normal = mat3(transpose(inverse(model)))*aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}


////+++++++caculate the specular in view space++++++
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//uniform vec3 lightPos;
//
//out vec3 Normal;
//out vec3 FragPos;
//out vec3 view_lightPos;
//
//void main()
//{
//	view_lightPos = vec3(view*vec4(lightPos,1.0));
//	FragPos = vec3(model*view*vec4(aPos,1.0));
//	Normal = mat3(transpose(inverse(model*view)))*aNormal;
//	gl_Position = projection * view * model * vec4(aPos, 1.0);
//}


////+++++++++ Ground Lighting Shader++++++++
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNormal;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform vec3 lightPos;
//uniform vec3 objectColor;
//uniform vec3 lightColor;
//
//out vec3 lightingColor;
//
//void main()
//{
//	vec3 view_lightPos = vec3(view*vec4(lightPos,1.0));
//	vec3 FragPos = vec3(model*view*vec4(aPos,1.0));
//	vec3 Normal = mat3(transpose(inverse(model*view)))*aNormal;
//	gl_Position = projection * view * model * vec4(aPos, 1.0);
//
//	float ambientStrength=0.1;
//	vec3 ambient = ambientStrength*lightColor;
//
//	vec3 norm = normalize(Normal);
//	vec3 lightDir = normalize(view_lightPos-FragPos);
//
//	float diff = max(dot(norm,lightDir),0.0);
//	vec3 diffuse = diff*lightColor;
//
//	float specularStrength = 0.5; 	//the power of specular
//	float gloss = 64;  	//the smoothness of materials
//	vec3 viewDir = normalize(-FragPos);
//	vec3 reflectDir = reflect(-lightDir,norm);
//	
//	float spec = pow(max(dot(viewDir,reflectDir),0.0),gloss);
//	vec3 specular = specularStrength*spec*lightColor;
//
//	lightingColor = ambient+diffuse+specular;
//}
