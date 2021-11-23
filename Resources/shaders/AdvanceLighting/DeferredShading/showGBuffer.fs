


#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform int selection = 0;
uniform sampler2D positionTex;
uniform sampler2D normalTex;
uniform sampler2D albedoSpecTex;

struct Light{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;
uniform bool gammaCorrect = false;
uniform int lightingMode = 0;

const float gamma = 2.2f;
const float invGamma = 0.454546f; // 1.0/gamma
vec4 DecodeGamma(vec4 src){
    return pow(src,vec4(invGamma));
}

void main(){
	switch(selection){
        case 0:
            //采样GBuffer
            vec3 fragPos = texture(positionTex,TexCoords).rgb;
            vec3 normal = texture(normalTex,TexCoords).rgb;
            vec4 albedoSpec = texture(albedoSpecTex,TexCoords);
            // 计算光照
            vec3 lighting = albedoSpec.rgb*0.1; // 硬编码环境光分量
            vec3 viewDir = normalize(viewPos-fragPos);

            //dir Light
            vec3 lightDir = normalize(-dirLight.direction);
            float diff = max(dot(lightDir,normal),0);
            float spec = 0;
            if(lightingMode == 0){ //Phong
                vec3 reflectDir = reflect(-lightDir,normal);
                spec = pow(max(dot(reflectDir,viewDir),0.0),16.0);
            }
            else if(lightingMode == 1){ //Blinn
                vec3 halfDir = normalize(lightDir+viewDir);
                spec = pow (max(dot(normal,halfDir),0.0),16.0);
            }
            vec3 diffuse = dirLight.diffuse.rgb*diff*albedoSpec.rgb;
            vec3 specular = dirLight.specular.rgb*spec*albedoSpec.a;
            lighting += diffuse+specular;

            // point Light
            for(int i=0;i<NR_LIGHTS; i++){
                // 光体积制约
                float distance = length(lights[i].Position-fragPos);
                if(distance<lights[i].Radius){
                     //漫反射
                    vec3 lightDir = normalize(lights[i].Position-fragPos);
                    vec3 diffuse = max(dot(normal,lightDir),0.0)*albedoSpec.rgb*lights[i].Color;
                    //高光
                    float spec = 0;
                    if(lightingMode == 0){ //Phong
                        vec3 reflectDir = reflect(-lightDir,normal);
                        spec = pow(max(dot(reflectDir,viewDir),0.0),16.0);
                    }
                    else if(lightingMode == 1){ //Blinn
                        vec3 halfDir = normalize(lightDir+viewDir);
                        spec = pow (max(dot(normal,halfDir),0.0),16.0);
                    }
                    vec3 specular = lights[i].Color*spec*albedoSpec.a;
                    //衰减
                    float lightDis = length(lights[i].Position - fragPos);
                    float attenuation = 1.0/(1.0 + lights[i].Linear*lightDis + lights[i].Quadratic* lightDis*lightDis);
                    //总和
                    diffuse *= attenuation;
                    specular*= attenuation;
                    lighting += diffuse+specular;
                }
            }
            FragColor = vec4 (lighting,1.0);
            break;
        case 1:
            FragColor = vec4(texture(positionTex,TexCoords).rgb,1.0);
            break;
        case 2:
            FragColor = vec4(texture(normalTex,TexCoords).rgb,1.0);
            break;
        case 3:
            FragColor = vec4(texture(albedoSpecTex,TexCoords).rgb,1.0);
            break;
        case 4:
            float specularColor = texture(albedoSpecTex,TexCoords).a;
            FragColor = vec4(specularColor,specularColor,specularColor,1.0);
            break;
    }
	if(gammaCorrect){
        FragColor = DecodeGamma(FragColor);
    }
}