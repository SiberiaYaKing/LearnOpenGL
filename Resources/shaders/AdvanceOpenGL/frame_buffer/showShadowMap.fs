#version 330 core
out vec4 depthColor;
in vec2 TexCoords;

uniform sampler2D depthMap;

void main(){
    float depthValue = texture(depthMap,TexCoords).r;
    depthColor = vec4(vec3(depthValue),1.0);
}