#pragma once

#include <glad/glad.h>
#include <iostream>
#include <vector>
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
	Framebuffer(unsigned bufferWidth,unsigned bufferHeight,unsigned autoAttachBufferCount=1) {
		//unsigned int framebuffer;
		glGenFramebuffers(1, &framebuffer);
		for (int i = 0; i < autoAttachBufferCount; i++) {
			attachBuffer(bufferWidth, bufferHeight,i);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//针对深度缓冲和模板缓冲不用采样的优化手段，生成并设置渲染缓冲
		glGenRenderbuffers(1, &depthStencilBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bufferWidth, bufferHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0); //reset
		//附加到缓冲
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilBuffer);
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

	void attachBuffer(unsigned bufferWidth, unsigned bufferHeight, unsigned index,GLuint textureFormat = GL_RGB ,GLuint channelFormat = GL_RGB, GLuint dataType = GL_UNSIGNED_BYTE) {
		increaseBufferID(index);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//生成纹理
		glGenTextures(1, &colorBuffers[index]);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[index]);
		glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, bufferWidth, bufferHeight, 0, channelFormat, dataType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);  //reset
		//附加到帧缓冲
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[index], GL_TEXTURE_2D, colorBuffers[index], 0);
		glDrawBuffers(attachments.size(), attachments.data());
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //reset
	}

	int getColorBuffer(GLuint index) const{
		if (index < colorBuffers.size()) {
			return colorBuffers[index];
		}
		else return colorBuffers.back();
	}


	inline int getFrameBuffer() const { 
		return framebuffer; 
	}

	void switch2Framebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	}

	void switch2Defaultbuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//手动清空帧缓冲
	void clearColor(float r=1,float g=1,float b=1,float a=1) {
		glClearColor(r,g,b,a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	void drawFramebuffer2Defaultbuffer(Shader screenShader) {
		screenShader.use();
		glBindVertexArray(screenVAO);
		for (GLuint colorBuffer : colorBuffers) {
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

private:
	class VGC {
		unsigned int VAO, VBO;
	public:
		VGC(unsigned VAO, unsigned VBO) :VAO(VAO), VBO(VBO) {}
		~VGC() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};


	void increaseBufferID(unsigned index) {
		colorBuffers.push_back(index);
		attachments.push_back(GL_COLOR_ATTACHMENT0 + index);
	}

private:
	GLuint framebuffer;
	GLuint depthStencilBuffer;
	GLuint screenVAO, screenVBO;
	std::vector<GLuint> colorBuffers;
	std::vector<GLuint> attachments;
};