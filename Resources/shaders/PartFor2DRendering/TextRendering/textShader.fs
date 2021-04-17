#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textTex;
uniform vec4 textColor;

void main(){
    float textAlpha = texture(textTex,TexCoords).r;
    FragColor = vec4(textColor.xyz,textAlpha);
}