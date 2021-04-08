#version 330 core
layout (location=0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main(){
	vec3 modelPos = normalize(aPos);
	texCoords = modelPos;
	vec4 pos = projection*view*vec4(modelPos,1.0);
	gl_Position = pos.xyww;
}