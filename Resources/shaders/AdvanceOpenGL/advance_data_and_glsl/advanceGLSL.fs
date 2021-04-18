#version 330 core
out vec4 FragColor;

in VS_OUT{
    vec2 TexCoords;
} fs_in;


uniform int selection = 0;
uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main(){
    switch(selection){
    case 0:
        FragColor = vec4(1,0,0.5,0);
        break;
    case 1:
        if(gl_FragCoord.x<400)
            FragColor = vec4(1,0,0,1);
        else
            FragColor = vec4(0,1,0,1);
        break;
    case 2:
        if(gl_FrontFacing)
            FragColor = texture(frontTexture, fs_in.TexCoords);
        else
            FragColor = texture(backTexture, fs_in.TexCoords);
        break;
    }
    
}