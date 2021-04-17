#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <LearnOpenGL/shader.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ft2build.h>

#include FT_FREETYPE_H


struct CharacterObj {
	GLuint textureID;	//字形纹理ID
	glm::ivec2 size;	//字形大小
	glm::ivec2 bearing;	// 从基准线到字形左部/顶部的偏移值
	GLuint advance;		//原点距下一个字形原点的距离
};



class Text {

private:
	class FTGC {
		FT_Library ft;
		FT_Face face;
		GLuint VAO, VBO;
	public:
		FTGC(FT_Library ft, FT_Face face, GLuint VAO, GLuint VBO) 
			:ft(ft), face(face),VAO(VAO),VBO(VBO) {}
		~FTGC() {
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}
	};

public :
	Text(const std::string& fontFilePath);
	void drawText(Shader& shader, const std::wstring& text, 
		glm::vec4 color, glm::vec2 position, GLfloat size);

private:
	bool loadCharacter(wchar_t character); 

public:
	std::unordered_map<wchar_t, CharacterObj> charMap;

private:
	FT_Library ft;
	FT_Face face;
	GLuint textVAO, textVBO;

};


Text::Text(const std::string& fontFilePath) {
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}
	if (FT_New_Face(ft, fontFilePath.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStoref(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
	// generate vao & vbo
	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	//bind vao & vbo
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	//set data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	//unbind vao & vbo
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	static FTGC gc(ft, face, textVAO, textVBO);
}

bool Text::loadCharacter(wchar_t character) {
	if (charMap.find(character) == charMap.end()) {
		// 加载字符的字形 
		if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			return false;
		}
		//生成纹理
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
			face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 储存字符供之后使用
		CharacterObj charObj = {
			textureId,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		charMap[character] = charObj;
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return true;
}

void Text::drawText(Shader& shader, const std::wstring& text,
	glm::vec4 color, glm::vec2 position, GLfloat size){
	shader.use();
	shader.setVec4("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);
	for (wchar_t character : text) {
		if (!loadCharacter(character)) return;
		CharacterObj charObj = charMap[character];
		GLfloat textQuadOriginPosX = position.x + charObj.bearing.x * size;
		GLfloat textQuadOriginPosY = position.y - (charObj.size.y - charObj.bearing.y) * size;
		GLfloat textQuadWidth = charObj.size.x * size;
		GLfloat textQuadHeight = charObj.size.y * size;
		glm::vec4 vertices[6] = {
		   { textQuadOriginPosX,					textQuadOriginPosY + textQuadHeight,	0.0, 0.0 },
		   { textQuadOriginPosX,					textQuadOriginPosY,						0.0, 1.0 },
		   { textQuadOriginPosX + textQuadWidth,	textQuadOriginPosY,						1.0, 1.0 },

		   { textQuadOriginPosX,					textQuadOriginPosY + textQuadHeight,	0.0, 0.0 },
		   { textQuadOriginPosX + textQuadWidth,	textQuadOriginPosY,						1.0, 1.0 },
		   { textQuadOriginPosX + textQuadWidth,	textQuadOriginPosY + textQuadHeight,	1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, charObj.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, _countof(vertices));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		position.x += (charObj.advance >> 6) * size; //advance的单位是1/64像素(2>>6 = 64)
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

