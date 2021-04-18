#pragma once
#include <stb_image.h>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <exception>

class TextureLoadExeception :public std::exception {
public:
	std::string img_path;
	TextureLoadExeception() {}
	TextureLoadExeception(const std::string& img_path) :img_path(img_path) {};
	const char* what()const throw() {
		return "Failed to load textureID ";
	}

};


class TextureLoader {
public:
	TextureLoader(GLenum target = GL_TEXTURE_2D) :target(target) {}

	TextureLoader(const std::string &img_path,GLuint unit=0 ,GLenum target = GL_TEXTURE_2D) :target(target),textureUnit(unit) {
		initTexture(img_path);
	}

	TextureLoader(const std::vector<std::string> &img_paths, GLuint unit=0,GLenum target = GL_TEXTURE_CUBE_MAP):target(target),textureUnit(unit) {
		initCubeMap(img_paths);
	}

	void initCubeMap(const std::vector<std::string> &img_paths) {
		try {
			glGenTextures(1, &textureID);
			glBindTexture(target, textureID);
			int width, height, nrChannels;
			unsigned char* data = nullptr;
			for (unsigned int i = 0; i < img_paths.size(); i++) {
				data = stbi_load(img_paths[i].c_str(), &width, &height, &nrChannels, 0);
				if (data) {
					GLenum format;
					if (nrChannels == 1) format = GL_RED;
					else if (nrChannels == 3) format = GL_RGB;
					else if (nrChannels == 4) format = GL_RGBA;
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				}
				else throw TextureLoadExeception(img_paths[i]);
				stbi_image_free(data);
			}
		}
		catch (TextureLoadExeception tle) { std::cout << tle.what() << tle.img_path << std::endl; }
	}

	void initTexture(const std::string &img_path) {
		try { this->_initTexture_(img_path); }
		catch (TextureLoadExeception tle) { std::cout << tle.what() <<img_path<< std::endl; }
	}

	void setWrapping(GLint param = GL_REPEAT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
	}

	void setFiltering(GLint param = GL_LINEAR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	}

	void setMipmapFiltering(GLint param = GL_LINEAR_MIPMAP_LINEAR) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
	}

	void bindTexture() {
		glBindTexture(target, textureID);
	}

	inline unsigned int getTextureID() const {
		return  textureID;
	}
	inline unsigned int getTextureUnit() const {
		return textureUnit;
	}

	void static activeTexture(GLenum texcoord) {
		glActiveTexture(texcoord);
	}

	inline void activeAndBind() {
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(target, textureID);
	}
	inline void unactiveAndUnbind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(target, 0);
	}

private:
	unsigned int textureID;
	GLenum target;
	unsigned int textureUnit;
	
	void _initTexture_(const std::string &img_path) {
		glGenTextures(1, &textureID);
		glBindTexture(target, textureID);

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

			setFiltering();
			setWrapping();
			setMipmapFiltering();
		}
		else throw TextureLoadExeception();
		stbi_image_free(data);
	}

};