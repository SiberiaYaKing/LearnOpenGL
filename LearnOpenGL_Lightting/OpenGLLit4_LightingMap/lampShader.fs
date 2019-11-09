#version 330 core
out vec4 FragColor;

uniform vec3 lightDiff;

void main(){
	FragColor = vec4(lightDiff,1.0);
}