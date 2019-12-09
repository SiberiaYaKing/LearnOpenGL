#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform int selection=1;

void main()
{             
	
    vec3 I = normalize(Position - cameraPos);
	vec3 R;
	switch (selection){
		case 0:
		    R = reflect(I, normalize(Normal));
			break;
		case 1:
			float ratio = 1.00/1.52;
			R = refract(I,normalize(Normal),ratio);
			break;
		default:break;
	}

    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}