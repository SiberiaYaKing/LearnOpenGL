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

//�������ܴ��ڵ����⣺���ܲ�֧��Ĭ��û�������ģ��





//+++++++++++++++++++++++++++ Model ++++++++++++++++++++++++++++++++++++++++

class Model {
private:
	std::vector<Texture> textures_loaded;
public:
	Model(const char *path) {
		loadModel(path);
	}
	Model(const std::string &path) {
		loadModel(path);
	}
	void Draw(Shader shader);
private:
	//ģ������
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(const std::string &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

};

//---------------------------------------------------------------

void Model::Draw(Shader shader) {
	for (unsigned i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(const std::string &path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags&AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene*scene) {
	//�������ڵ��е���������(����еĻ�)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//�������������ӽڵ��ظ���һ����(�ݹ�)
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	std::vector<Texture> textures;
	//������
	for (unsigned i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		//������λ�á����ߺ���������
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if(mesh->mTextureCoords[0])
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else vertex.TexCoords = glm::vec2(0.0f);
		vertices.push_back(vertex);
	}
	//��������Ķ�������
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];  //�������γɵ��棬�����ξ�����������
		//��������
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	//�������
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];  //��ȡ���в���
		//��ȡ�����������ͼ
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//��ȡ�߹������ͼ
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
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
			if (textures_loaded[j].path == str) {
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
			texture.path = str;
			textures.push_back(texture);
		}
	}
	return textures;
}