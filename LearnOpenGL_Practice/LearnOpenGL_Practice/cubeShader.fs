#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture1;

float near = 0.1;
float far = 100;

float LinearizeDepth(float depth){
	float z = depth *2.0 -1.0;
	return (2.0*near*far)/(far+near-z*(far-near));
}


void main(){
	FragColor = texture(texture1,TexCoords);
	//FragColor = vec4(vec3(gl_FragCoord.z),1.0);
	//float depth = LinearizeDepth(gl_FragCoord.z)/far;
	//FragColor = vec4(vec3(depth),1.0);
}