#pragma once
#include "types.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
	Model(const char* path)
	{
		LoadModel(path);
	};
	~Model();

	std::vector<Vertex> allVertices;

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::string directory;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};