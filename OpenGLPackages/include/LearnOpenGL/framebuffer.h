#pragma once

#include <glad/glad.h>
#include <iostream>
#include <LearnOpenGL/shader.h>

const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};



class Framebuffer {
public:
	Framebuffer(unsigned scr_width,unsigned scr_height) {
		//unsigned int framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//生成纹理
		//unsigned int texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scr_width, scr_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);  //reset
		//附加到帧缓冲
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

		//render buffer
		//------------------------
		//针对深度缓冲和模板缓冲不用采样的优化手段，生成并设置渲染缓冲
		//unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scr_width, scr_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0); //reset
		//附加到缓冲
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		//检查帧缓冲是否完整
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //reset

		//绑定顶点数据
		glGenVertexArrays(1, &screenVAO);
		glGenBuffers(1, &screenVBO);
		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
		
		static VGC gc(screenVAO, screenVBO);
	}

	void switch2Framebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
	}

	void switch2Defaultbuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	}

	//不太清楚封装的意义，慎用
	void clearColor(float r=1,float g=1,float b=1,float a=1) {
		glClearColor(r,g,b,a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	void drawFramebuffer2Defaultbuffer(Shader screenShader) {
		screenShader.use();
		glBindVertexArray(screenVAO);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	class VGC {
		unsigned int VAO, VBO;
	public:
		VGC(unsigned VAO, unsigned VBO) :VAO(VAO), VBO(VBO) {}
		~VGC() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};

private:
	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;
	unsigned int screenVAO, screenVBO;
};