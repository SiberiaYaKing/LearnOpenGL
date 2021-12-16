#pragma once
#include <map>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <LearnOpenGL/shader.h>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <LearnOpenGL/framebuffer.h>


//�������ܴ��ڵ����⣺��ɫ�������ܶ�̬��

const std::string TEXTURE_DIFFUSE = "texture_diffuse";
const std::string TEXTURE_SPECULAR = "texture_specular";

const std::string SHADER_DEFAULT = "Default";
const std::string SHADER_PBR = "PBR";

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
	//��������
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(const std::vector<Vertex>&, const std::vector<unsigned int>&,const std::vector<Texture>&);

	inline unsigned int getVAO() const { return VAO; }
	
	void Draw(Shader shader , const std::string& shaderType =SHADER_DEFAULT);
	void drawPoint(Shader shader);
	void drawInstance(Shader shader, GLsizei count, const std::string& shaderType=SHADER_DEFAULT);
private:
	//��Ⱦ����
	unsigned int VAO, VBO, EBO;

	void setupMesh();

	void setupDefaultShader(Shader& shader);
	void setupPBRShader(Shader& shader);

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

	std::map<std::string, std::function<void(Shader&)> > setupShaderFuncMap;
	inline void setupFunMap() {
		setupShaderFuncMap = {
		{SHADER_DEFAULT, std::bind(&Mesh::setupDefaultShader,*this,std::placeholders::_1)},
		{SHADER_PBR, std::bind(&Mesh::setupPBRShader,*this,std::placeholders::_1)},
		};
	}
};

//------------------------------------------------------------------

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
	setupFunMap();
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

	//����λ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); //���

	static MeshGC gc;
	gc.add_id(VAO, VBO, EBO);
}

void Mesh::Draw(Shader shader,const std::string& shaderType) {
	setupShaderFuncMap[shaderType](shader);

	//��������
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //���
	glBindTexture(GL_TEXTURE_2D, 0);

	//�����ͼ
	glActiveTexture(0);
}

void Mesh::drawPoint(Shader shader) {
	shader.use();
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); //���
}

void Mesh::drawInstance(Shader shader, GLsizei count, const std::string& shaderType) {
	setupShaderFuncMap[shaderType](shader);
	//��������
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0,count);
	glBindVertexArray(0); //���
	glBindTexture(GL_TEXTURE_2D, 0);

	//�����ͼ
	glActiveTexture(0);
}

void Mesh::setupDefaultShader(Shader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	shader.use();
	//�Ƿ�����ͼ
	if (textures.size()) {
		shader.setBool("has_texture", true);
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			std::string number;
			std::string name = textures[i].type;
			if (name == TEXTURE_DIFFUSE)
				number = std::to_string(diffuseNr++);
			else if (name == TEXTURE_SPECULAR)
				number = std::to_string(specularNr++);
			shader.setInt(name + number, i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}
	else  shader.setBool("has_texture", false);

	float shininess = 0.25f;
	shader.setFloat("shininess", shininess * 128.0f);
}

void Mesh::setupPBRShader(Shader& shader) {
	//TODO: for support pbr
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
