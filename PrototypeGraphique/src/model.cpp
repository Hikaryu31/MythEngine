#include "model.h"
#include <iostream>
#include <stb_image.h>
#include <filesystem>

Model::~Model()
{
	if (loadedTextures.size() != 0)
		glDeleteTextures(1, &loadedTextures[0].gpu->data);

	for (int i = loadedTextures.size() - 1; i >= 0; i--)
	{
		delete(loadedTextures[i].gpu);
	}
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	std::string pathos = std::filesystem::canonical(path).string();
	const aiScene* scene = importer.ReadFile(pathos, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

void ProcessVertex(Vertex& vertex, aiMesh* mesh, unsigned int indice)
{
	vertex.position.x = mesh->mVertices[indice].x;
	vertex.position.y = mesh->mVertices[indice].y;
	vertex.position.z = mesh->mVertices[indice].z;

	vertex.normal.x = mesh->mNormals[indice].x;
	vertex.normal.y = mesh->mNormals[indice].y;
	vertex.normal.z = mesh->mNormals[indice].z;

	if (mesh->mTextureCoords[0])
	{
		vertex.uv.x = mesh->mTextureCoords[0][indice].x;
		vertex.uv.y = mesh->mTextureCoords[0][indice].y;
	}
	else
		vertex.uv = v2{ 0.f, 0.f };
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		ProcessVertex(vertex, mesh, i);
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
	return Mesh();
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.gpu = new GPUTexture;
			texture.gpu->data = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
	}

	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	int width, height, nrChannels;
	unsigned int textureID;

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	glGenTextures(1, &textureID);

	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	else
	{
		fprintf(stderr, "Image loading failed on '%s'\n", path);
	}

	stbi_image_free(data);
	return textureID;
}
