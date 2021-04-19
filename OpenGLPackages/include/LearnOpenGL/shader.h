#pragma once
#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class UniformBuffer {
public:
	UniformBuffer(GLuint shaderID, const std::string& name, GLuint bindingPoint, GLsizeiptr bufferSize) {
		init({ shaderID, }, { name, }, bindingPoint, bufferSize);
	}

	UniformBuffer(const std::vector<GLuint> shaderIDs, const std::vector<std::string>& names, GLuint bindingPoint, GLsizeiptr bufferSize) {
		init(shaderIDs, names, bindingPoint, bufferSize);
	}

	inline void setSubData(GLsizeiptr offset, GLsizeiptr size, const void* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

private:
	void init(const std::vector<GLuint> shaderIDs, const std::vector<std::string>& names, GLuint bindingPoint, GLsizeiptr bufferSize) {
		for (int i = 0; i < shaderIDs.size(); i++) {
			GLuint uniformBlockIndex = glGetUniformBlockIndex(shaderIDs[i], names[i].c_str());
			glUniformBlockBinding(shaderIDs[i], uniformBlockIndex, bindingPoint);
		}
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, 0, bufferSize);
	}

private:
	GLuint UBO;
};

class Shader
{
public:
	// 程序ID
	unsigned int ID;

	// 构造器读取并构建着色器
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	// 使用/激活程序
	void use();
	// uniform工具函数
	inline void setBool(const std::string &name, bool value) const;
	inline void setInt(const std::string &name, int value) const;
	inline void setFloat(const std::string &name, float value) const; 
	inline void setMat4(const std::string &name, glm::mat4 value)const;
	inline void setVec3(const std::string &name, glm::vec3 value) const;
	inline void setVec3(const std::string &name, float f1,float f2,float f3) const;
	inline void setMat3(const std::string &name, glm::mat3 value)const;
	inline void setVec4(const std::string& name, glm::vec4 value) const;
	inline void setVec4(const std::string& name, float f1, float f2, float f3, float f4) const;
private:
	void init_shader(const GLchar* vertexPath, const GLchar*  fragmentPath);
};

void Shader::init_shader(const GLchar* vertexPath, const GLchar*  fragmentPath) {
	//1.从文件路径中获取顶点/片元着色器
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//保证ifstream对象可以抛出异常：
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		//打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//读取文件的缓冲内容到数据流中
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//关闭文件处理器
		vShaderFile.close();
		fShaderFile.close();
		//转换数据流到string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//2.编译着色器
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//片元着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//着色器程序
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//删除着色器，它们已经链接到我们的程序中了，不需要了
	glDeleteShader(vertex);
	glDeleteShader(vertex);
}

Shader::Shader(const GLchar* vertexPath, const GLchar*  fragmentPath) {
	init_shader(vertexPath, fragmentPath);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
	init_shader(vertexPath.c_str(), fragmentPath.c_str());
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name,bool value)const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string & name, glm::mat4 value)const{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1,GL_FALSE,glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec3(const std::string &name, float f1, float f2, float f3) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), f1, f2, f3);
}

void Shader::setMat3(const std::string &name, glm::mat3 value)const {
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, glm::vec4 value) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x,value.y,value.z,value.w);
}

void Shader::setVec4(const std::string& name, float f1, float f2, float f3, float f4) const {
	glUniform4f(glGetUniformLocation(ID, name.c_str()), f1, f2, f3, f4);
}

