#pragma once

#include <vector>
#include <string>
#include <LearnOpenGL/shader.h>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <LearnOpenGL/mesh.h>
#include <LearnOpenGL/texture_loader.h>

//+++++++++++++++++++++++++++ Model ++++++++++++++++++++++++++++++++++++++++

class Model {
private:
	std::vector<Texture> textures_loaded;
public:
	Model(const char *path , bool isCorrectTangent = false) {
		this->isCorrectTangent = isCorrectTangent;
		loadModel(path);
	}
	Model(const std::string &path , bool isCorrectTangent = false) {
		this->isCorrectTangent = isCorrectTangent;
		loadModel(path);
	}
	void Draw(Shader shader, const std::string& shaderType = SHADER_DEFAULT);
	void drawInstance(Shader shader, GLsizei count, const std::string& shaderType = SHADER_DEFAULT);
	inline void getMeshes(std::vector<Mesh>& outMeshes) const { outMeshes = meshes; }

private:
	//模型数据
	std::vector<Mesh> meshes;
	std::string directory;

	bool isCorrectTangent = false;

	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

//---------------------------------------------------------------


void Model::Draw(Shader shader, const std::string& shaderType) {
	for (unsigned i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader,shaderType);
}


void Model::drawInstance(Shader shader, GLsizei count, const std::string& shaderType) {
	for (unsigned i = 0; i < meshes.size(); i++)
		meshes[i].drawInstance(shader, count, shaderType);
}

void Model::loadModel(const std::string &path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs  | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene*scene) {
	//处理单个节点中的所有网格(如果有的话)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//接下来对它的子节点重复这一过程(递归)
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	std::vector<Texture> textures;
	//处理顶点
	for (unsigned i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		//处理顶点位置、法线和纹理坐标
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		if (mesh->mTextureCoords[0]) {
			float x = mesh->mTextureCoords[0][i].x;
			float y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = glm::vec2(x, y);
		}
		else vertex.TexCoords = glm::vec2(0.0f);
		vertices.push_back(vertex);
	}
	std::vector<Vertex *> faceVertices;
	//处理构成面的顶点索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];  //由索引形成的面，三角形就是三个索引
		faceVertices.clear();
		//处理索引
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			int index = face.mIndices[j];
			faceVertices.push_back(&vertices[index]);
			if (isCorrectTangent) {
				float x = faceVertices[j]->TexCoords.x;
				float y = faceVertices[j]->TexCoords.y;
				x = x > 1 ? x - 1 : x;
				y = y > 1 ? y - 1 : y;
				faceVertices[j]->TexCoords.x = x;
				faceVertices[j]->TexCoords.y = y;
			}
			indices.push_back(index);
		}
		if (isCorrectTangent) {
			glm::vec3 tangent = { 0,0,0 };
			glm::vec3 edge1 = faceVertices[1]->Position - faceVertices[0]->Position;
			glm::vec3 edge2 = faceVertices[2]->Position - faceVertices[0]->Position;
			glm::vec2 deltaUV1 = faceVertices[1]->TexCoords - faceVertices[0]->TexCoords;
			glm::vec2 deltaUV2 = faceVertices[2]->TexCoords - faceVertices[0]->TexCoords;
			//GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
			tangent.x = /*f **/ (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = /*f **/ (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = /*f **/ (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent = glm::normalize(tangent);
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				faceVertices[j]->Tangent = tangent;
			}
		}
	}
	
	//处理材质
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];  //获取所有材质
		//获取漫反射材质贴图
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//获取高光材质贴图
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		//获取法线贴图
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool is_skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) 
			if (textures_loaded[j].path == str.C_Str()) {
				textures.push_back(textures_loaded[j]);
				is_skip = true;
				break;
			}
		if (!is_skip) {
			Texture texture;
			std::string full_path = directory + '/' + str.C_Str();
			TextureLoader tLoader(full_path);
			texture.id = tLoader.getTextureID();
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
	}
	return textures;
}