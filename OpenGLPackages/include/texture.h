#pragma once
#include <stb_image.h>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <exception>

class TextureLoadExeception :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to load texture";
	}
};

class Texture {
public:
	Texture(GLenum target = GL_TEXTURE_2D) :target(target) {}

	void initTexture(const std::string &img_path) {
		try { this->_initTexture_(img_path); }
		catch (TextureLoadExeception tle) { std::cout << tle.what() << std::endl; }
	}

	void SetWrapping(GLint param = GL_REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
	}

	void SetFiltering(GLint param = GL_LINEAR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	}

	void SetMipmapFiltering(GLint param = GL_LINEAR_MIPMAP_LINEAR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	}

	void ActiveAndBind(GLenum texcoord) {
		glActiveTexture(texcoord);
		glBindTexture(target, texture);
	}

private:
	unsigned int texture;
	GLenum target = GL_TEXTURE_2D;

	void _initTexture_(const std::string &img_path) {
		glGenTextures(1, &texture);
		glBindTexture(target, texture);

		SetFiltering();
		SetWrapping();
		SetMipmapFiltering();

		int width, height, nrChannels;
		unsigned char*data = stbi_load(img_path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			switch (nrChannels) {
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				break;
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				break;
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else throw TextureLoadExeception();
		stbi_image_free(data);
	}
};