


#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform int selection = 1;
uniform sampler2D positionTex;
uniform sampler2D normalTex;
uniform sampler2D albedoSpecTex;

void main(){
	switch(selection){
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
	
}