# version 330 core
layout (location = 0) in vec4 vertexData; //<vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main(){
    gl_Position = projection*vec4(vertexData.xy,0.0,1.0);
    TexCoords = vertexData.zw;
}