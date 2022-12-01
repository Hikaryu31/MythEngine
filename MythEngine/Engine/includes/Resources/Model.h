#pragma once

#include "define.h"
#include "Common.h"

#include "Shader.h"

#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Resources
{
	struct GPUMesh
	{
		unsigned int VertexCount = 0u;
		GLuint VAO = 0u;
		GLuint VBO = 0u;
	};

	class Mesh
	{
	public:
		Mesh(const std::string& p_name) : Name(p_name), MaterialName("default") {};
		~Mesh() { UnloadVAO(); };

		std::string Name;
		std::string MaterialName;
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		GPUMesh Gpu;

		void LoadVAO();
		void UnloadVAO();
	};

	struct TextureInfo
	{
		Vec4 color = { 0.f, 0.f, 0.f, 1.f };
		std::string name = "";
		unsigned char* texel = nullptr;
		bool isEmbedded = false;
		int width = 0;
		int height = 0;
	};

	struct DefaultMaterial
	{
		float m_shininess = 1.f;

		TextureInfo m_ambient;
		TextureInfo m_diffuse;
		TextureInfo m_specular;
		TextureInfo m_emissive;
		TextureInfo m_normalMap;
	};

	struct BoneInfo
	{
		unsigned int id = 0;
		Mat4x4 offsetMatrix;
	};

	// ----- Model -----

	class Model : public Resource
	{
	public:
		Model(const std::string& p_name);
		~Model() {}

		REGISTER_RESOURCE_HPP()

		std::string Path = "";
		std::vector<Mat4x4> m_boneTransforms;

		void LoadModel();

		const std::vector<Mesh>& GetMeshs() const { return m_meshs; }
		const std::unordered_map<std::string, DefaultMaterial>& GetDefaultMaterials() const { return m_defaultMaterial; }
		const std::vector<Mat4x4>& GetDefaultLocalMatrix() const { return m_defaultLocalMatrix; }
		const std::string& GetDirectory() const { return m_directory; }
		const aiScene* GetAssimpScene() const { return m_scene; }
		const std::unordered_map<std::string, BoneInfo>& GetBoneMap() const { return m_boneInfoMap; };

		void LoadResource() override;
		void UnloadResource() override;
#ifdef MODE_EDITOR
		void ShowEditorControl() override;
#endif // MODE_EDITOR

	private:
		const aiScene* m_scene = nullptr;
		Assimp::Importer m_importer;
		std::string m_directory = "";

		std::vector<Mesh> m_meshs;
		std::unordered_map<std::string, DefaultMaterial> m_defaultMaterial;
		std::vector<Mat4x4> m_defaultLocalMatrix;
		std::unordered_map<std::string, BoneInfo> m_boneInfoMap;
		unsigned int m_boneCounter = 0;

		void ProcessNode(aiNode* p_node);
		void ProcessMesh(const std::string& p_meshName, const aiMesh* p_mesh);
		void ProcessMaterials();
		void ExtractBoneWeightForVertices(std::vector<Vertex>& p_vertices, const aiMesh* p_mesh);
	};

	Mat4x4 AiMatrixToMat4x4(const aiMatrix4x4& p_matrix);
}