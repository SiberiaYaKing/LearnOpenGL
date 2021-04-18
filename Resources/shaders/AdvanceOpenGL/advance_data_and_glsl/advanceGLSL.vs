#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform float pointSize = 1;

layout (std140) uniform Matrices{
    mat4 view;
    mat4 projection;
};

//接口块（类似hlsl的结构体
out VS_OUT{
    vec2 TexCoords;
} vs_out;

void main(){
    gl_Position = projection * view *model*vec4(aPos,1.0);
    gl_PointSize = gl_Position.z*pointSize; 
    vs_out.TexCoords = aTexCoords;
}