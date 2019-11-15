#version 330 core
out vec4 FragColor;

uniform vec3 lightDiff;

void main(){
	if(lightDiff.xyz==vec3(0.0))
		FragColor = vec4(1.0);
	else FragColor = vec4(lightDiff,1.0);
}