#pragma once
#include <vector>
#include <string>
#include <LearnOpenGL/shader.h>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

//这个类可能存在的问题：着色器纹理不能动态绑定

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent = glm::vec3(0);
	glm::vec3 Bitangent = glm::vec3(0);
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};



class Mesh {
public:
	//网格数据
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex>, std::vector<unsigned int>, std::vector<Texture>);

	void Draw(Shader shader);
	void drawPoint(Shader shader);
private:
	//渲染数据
	unsigned int VAO, VBO, EBO;

	void setupMesh();

private:
	class MeshGC {
	private:
		std::vector<unsigned int> VAOs, VBOs, EBOs;
	public:
		void add_id(unsigned VAO, unsigned VBO, unsigned EBO) {
			VAOs.push_back(VAO);
			VBOs.push_back(VBO);
			EBOs.push_back(EBO);
		}
		~MeshGC() {
			for (int i = 0; i < VAOs.size(); i++) {
				glDeleteVertexArrays(1, &VAOs[i]);
				glDeleteBuffers(1, &VBOs[i]);
				glDeleteBuffers(1, &EBOs[i]);
			}
		}
	};

};

//------------------------------------------------------------------

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//顶点位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); //解绑

	static MeshGC gc;
	gc.add_id(VAO, VBO, EBO);
}

void Mesh::Draw(Shader shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	shader.use();
	//是否有贴图
	if (textures.size()) {
		shader.setBool("has_texture", true);
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular") 
				number = std::to_string(specularNr++);
			shader.setInt( name + number, i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}
	else  shader.setBool("has_texture", false);

	float shininess = 0.25f;
	shader.setFloat("shininess", shininess*128.0f);

	//绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //解绑
	glBindTexture(GL_TEXTURE_2D, 0);

	//解绑贴图
	glActiveTexture(0);
}

void Mesh::drawPoint(Shader shader) {
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //解绑
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
