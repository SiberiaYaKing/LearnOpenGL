#pragma once
#include <stb_image.h>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <exception>

class TextureLoadExeception :public std::exception {
public:
	const char* what()const throw() {
		return "Failed to load texture ";
	}
};

class Texture {
public:
	Texture(GLenum target = GL_TEXTURE_2D) :target(target) {}

	void initTexture(const std::string &img_path) {
		try { this->_initTexture_(img_path); }
		catch (TextureLoadExeception tle) { std::cout << tle.what() <<img_path<< std::endl; }
	}

	void SetWrapping(GLint param = GL_REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
	}

	void SetFiltering(GLint param = GL_LINEAR) {
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

		int width, height, nrChannels;
		unsigned char*data = stbi_load(img_path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			SetFiltering();
			SetWrapping();
			SetMipmapFiltering();
		}
		else throw TextureLoadExeception();
		stbi_image_free(data);
	}
};