#pragma once

#include <glad/glad.h>
#include <string>

class UniformBuffer {
public:
	UniformBuffer(GLuint shaderID ,const std::string& name,GLuint bindingPoint,GLsizeiptr bufferSize) {
		GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderID,name.c_str());
		glUniformBlockBinding(shaderID, uniformBlockIndex, bindingPoint);
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, 0, bufferSize);
	}

	void setSubData(GLsizeiptr offset, GLsizeiptr size, const void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER,offset,size,data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

private:
	GLuint UBO;

};