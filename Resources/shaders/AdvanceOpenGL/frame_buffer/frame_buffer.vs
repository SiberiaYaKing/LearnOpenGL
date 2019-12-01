#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;


void main()
{	
	//The secret of the frame buffer's ability to render to the screen is this: 
	//   the shader does not transform the vertices, 
	//   and the coordinate space of the vertices is screen space(vertices in framebuffer.h)
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}