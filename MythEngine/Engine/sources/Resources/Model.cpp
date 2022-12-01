#include "Resources/Model.h"

#include "Core/System/Application.h"

#include <fmt/format.h>

// ===== MESH =====

void Resources::Mesh::LoadVAO()
{
	if (Gpu.VBO != 0u && Gpu.VAO != 0u)
		return;
	if (Gpu.VBO != 0u || Gpu.VAO != 0u)
		UnloadVAO();

	std::vector<Vertex> orderedVertices;
	for (int i = 0; i < Indices.size(); i++)
		orderedVertices.push_back(Vertices[Indices[i]]);

	Gpu.VertexCount = (unsigned int)orderedVertices.size();

	glGenBuffers(1, &Gpu.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Gpu.VBO);
	glBufferData(GL_ARRAY_BUFFER, Gpu.VertexCount * sizeof(Vertex), orderedVertices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &Gpu.VAO);
	glBindVertexArray(Gpu.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Gpu.VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	glBindVertexArray(0);
}

void Resources::Mesh::UnloadVAO()
{
	Gpu.VertexCount = 0u;

	if (Gpu.VBO != 0u)
	{
		glDeleteBuffers(1, &Gpu.VBO);
		Gpu.VBO = 0u;
	}

	if (Gpu.VAO != 0u)
	{
		glDeleteVertexArrays(1, &Gpu.VAO);
		Gpu.VAO = 0u;
	}
}

// ===== MODEL =====

REGISTER_RESOURCE_CPP(Resources::Model)

Resources::Model::Model(const std::string& p_name) : Resource(p_name), m_scene(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

Mat4x4 Resources::AiMatrixToMat4x4(const aiMatrix4x4& p_matrix)
{
	Mat4x4 matrix{
		p_matrix.a1, p_matrix.a2, p_matrix.a3, p_matrix.a4,
		p_matrix.b1, p_matrix.b2, p_matrix.b3, p_matrix.b4,
		p_matrix.c1, p_matrix.c2, p_matrix.c3, p_matrix.c4,
		p_matrix.d1, p_matrix.d2, p_matrix.d3, p_matrix.d4
	};

	return matrix.GetTransposed();
}

void Resources::Model::LoadModel()
{
	UnloadResource();
	m_meshs.clear();
	m_defaultLocalMatrix.clear();

	m_defaultMaterial.clear();
	m_defaultMaterial.insert({ "default", DefaultMaterial() });

	m_scene = m_importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
	{
		MYTH_ERROR("ASSIMP::%s", m_importer.GetErrorString());
		return;
	}

	m_directory = Path.substr(0, Path.find_last_of('/'));
	ProcessMaterials();
	ProcessNode(m_scene->mRootNode);
}

void AiColor4DToVec4(const aiColor4D& color, Vec4& newColor)
{
	newColor.m_x = color.r;
	newColor.m_y = color.g;
	newColor.m_z = color.b;
	newColor.m_w = color.a;
}

void SetVertexBoneData(Vertex& p_vertex, int p_boneID, float p_weight)
{
	for (int i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
	{
		if (p_vertex.weights[i] == 0.0)
		{
			p_vertex.boneIDs[i] = p_boneID;
			p_vertex.weights[i] = p_weight;
			return;
		}
	}
	MYTH_ASSERT(0);
}

void Resources::Model::ExtractBoneWeightForVertices(std::vector<Vertex>& p_vertices, const aiMesh* p_mesh)
{
	for (unsigned int boneIndex = 0; boneIndex < p_mesh->mNumBones; boneIndex++)
	{
		int boneID = -1;
		std::string boneName = p_mesh->mBones[boneIndex]->mName.C_Str();

		if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_boneCounter;
			newBoneInfo.offsetMatrix = AiMatrixToMat4x4(p_mesh->mBones[boneIndex]->mOffsetMatrix);
			m_boneInfoMap[boneName] = newBoneInfo;
			boneID = m_boneCounter;
			m_boneCounter++;
		}

		else
			boneID = m_boneInfoMap[boneName].id;

		MYTH_ASSERT(boneID != -1);

		if (boneID == m_boneTransforms.size())
			m_boneTransforms.push_back(AiMatrixToMat4x4(p_mesh->mBones[boneIndex]->mOffsetMatrix));

		auto weights = p_mesh->mBones[boneIndex]->mWeights;
		int numWeights = p_mesh->mBones[boneIndex]->mNumWeights;

		for (int i = 0; i < numWeights; i++)
		{
			int vertexID = weights[i].mVertexId;
			float weight = weights[i].mWeight;
			MYTH_ASSERT(vertexID <= p_vertices.size());

			SetVertexBoneData(p_vertices[vertexID], boneID, weight);
		}
	}
}

void ProcessVertex(Vertex& p_vertex, const aiMesh* p_mesh, unsigned int p_indice)
{
	p_vertex.position.m_x = p_mesh->mVertices[p_indice].x;
	p_vertex.position.m_y = p_mesh->mVertices[p_indice].y;
	p_vertex.position.m_z = p_mesh->mVertices[p_indice].z;

	p_vertex.normal.m_x = p_mesh->mNormals[p_indice].x;
	p_vertex.normal.m_y = p_mesh->mNormals[p_indice].y;
	p_vertex.normal.m_z = p_mesh->mNormals[p_indice].z;

	p_vertex.tangent.m_x = p_mesh->mTangents[p_indice].x;
	p_vertex.tangent.m_y = p_mesh->mTangents[p_indice].y;
	p_vertex.tangent.m_z = p_mesh->mTangents[p_indice].z;

	if (p_mesh->mTextureCoords[0])
	{
		p_vertex.uv.m_x = p_mesh->mTextureCoords[0][p_indice].x;
		p_vertex.uv.m_y = p_mesh->mTextureCoords[0][p_indice].y;
	}
	else
		p_vertex.uv = Vec2{ 0.f, 0.f };
}

void Resources::Model::ProcessNode(aiNode* p_node)
{
	for (unsigned int i = 0; i < p_node->mNumMeshes; i++)
	{
		aiMesh* mesh = m_scene->mMeshes[p_node->mMeshes[i]];

		std::string meshName = ("mesh_" + std::to_string(m_meshs.size()));
		if (mesh->mName.length > 0u)
			meshName = mesh->mName.C_Str();

		ProcessMesh(meshName, mesh);
		m_defaultLocalMatrix.push_back(AiMatrixToMat4x4(p_node->mTransformation));
	}

	for (unsigned int i = 0; i < p_node->mNumChildren; i++)
		ProcessNode(p_node->mChildren[i]);
}

void Resources::Model::ProcessMesh(const std::string& p_meshName, const aiMesh* p_mesh)
{
	m_meshs.emplace_back(p_meshName);
	Resources::Mesh& tmp_mesh = m_meshs.back();

	for (unsigned int i = 0; i < p_mesh->mNumVertices; i++)
	{
		tmp_mesh.Vertices.emplace_back();
		ProcessVertex(tmp_mesh.Vertices.back(), p_mesh, i);
	}

	for (unsigned int i = 0; i < p_mesh->mNumFaces; i++)
	{
		aiFace face = p_mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			tmp_mesh.Indices.push_back(face.mIndices[j]);
	}

	aiMaterial* material = m_scene->mMaterials[p_mesh->mMaterialIndex];
	if (material->GetName().length > 0u)
		tmp_mesh.MaterialName = material->GetName().C_Str();
	else
		tmp_mesh.MaterialName = "material_" + std::to_string(p_mesh->mMaterialIndex);

	ExtractBoneWeightForVertices(tmp_mesh.Vertices, p_mesh);
}

void Resources::Model::ProcessMaterials()
{
	for (unsigned int i = 0u; i < m_scene->mNumMaterials; i++)
	{
		aiMaterial* material = m_scene->mMaterials[i];

		std::string matName = "";
		if (material->GetName().length > 0u)
			matName = material->GetName().C_Str();
		else
			matName = "material_" + std::to_string(i);

		if (m_defaultMaterial.find(matName) != m_defaultMaterial.end())
			continue;

		m_defaultMaterial.insert({ matName, DefaultMaterial() });
		DefaultMaterial& currentMaterial = m_defaultMaterial[matName];

		aiColor4D ambient;
		aiColor4D diffuse;
		aiColor4D specular;
		aiColor4D emissive;

		aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
		AiColor4DToVec4(ambient, currentMaterial.m_ambient.color);

		if (material->GetTextureCount(aiTextureType_AMBIENT) > 0u)
		{
			aiString ambientTextureName;
			material->GetTexture(aiTextureType_AMBIENT, 0u, &ambientTextureName);
			if (auto texture = m_scene->GetEmbeddedTexture(ambientTextureName.C_Str()))
			{
				currentMaterial.m_ambient.isEmbedded = true;
				currentMaterial.m_ambient.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_ambient.width = texture->mWidth;
				currentMaterial.m_ambient.height = texture->mHeight;
			}
			currentMaterial.m_ambient.name = (m_directory + '/' + ambientTextureName.C_Str());
		}

		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		AiColor4DToVec4(diffuse, currentMaterial.m_diffuse.color);

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0u)
		{
			aiString diffuseTextureName;
			material->GetTexture(aiTextureType_DIFFUSE, 0u, &diffuseTextureName);

			if (auto texture = m_scene->GetEmbeddedTexture(diffuseTextureName.C_Str()))
			{
				currentMaterial.m_diffuse.isEmbedded = true;
				currentMaterial.m_diffuse.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_diffuse.width = texture->mWidth;
				currentMaterial.m_diffuse.height = texture->mHeight;
			}
			currentMaterial.m_diffuse.name = (m_directory + '/' + diffuseTextureName.C_Str());
		}

		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
		AiColor4DToVec4(specular, currentMaterial.m_specular.color);

		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0u)
		{
			aiString specularTextureName;
			material->GetTexture(aiTextureType_SPECULAR, 0u, &specularTextureName);
			if (auto texture = m_scene->GetEmbeddedTexture(specularTextureName.C_Str()))
			{
				currentMaterial.m_specular.isEmbedded = true;
				currentMaterial.m_specular.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_specular.width = texture->mWidth;
				currentMaterial.m_specular.height = texture->mHeight;
			}
			currentMaterial.m_specular.name = (m_directory + '/' + specularTextureName.C_Str());
		}

		aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emissive);
		AiColor4DToVec4(emissive, currentMaterial.m_emissive.color);

		if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0u)
		{
			aiString emissiveTextureName;
			material->GetTexture(aiTextureType_EMISSIVE, 0u, &emissiveTextureName);
			if (auto texture = m_scene->GetEmbeddedTexture(emissiveTextureName.C_Str()))
			{
				currentMaterial.m_emissive.isEmbedded = true;
				currentMaterial.m_emissive.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_emissive.width = texture->mWidth;
				currentMaterial.m_emissive.height = texture->mHeight;
			}
			currentMaterial.m_emissive.name = (m_directory + '/' + emissiveTextureName.C_Str());
		}

		if (material->GetTextureCount(aiTextureType_HEIGHT) > 0u)
		{
			aiString normalMapName;
			material->GetTexture(aiTextureType_HEIGHT, 0u, &normalMapName);
			if (auto texture = m_scene->GetEmbeddedTexture(normalMapName.C_Str()))
			{
				currentMaterial.m_normalMap.isEmbedded = true;
				currentMaterial.m_normalMap.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_normalMap.width = texture->mWidth;
				currentMaterial.m_normalMap.height = texture->mHeight;
			}
			currentMaterial.m_normalMap.name = (m_directory + '/' + normalMapName.C_Str());
		}

		else if (material->GetTextureCount(aiTextureType_NORMALS) > 0u)
		{
			aiString normalMapName;
			material->GetTexture(aiTextureType_NORMALS, 0u, &normalMapName);
			if (auto texture = m_scene->GetEmbeddedTexture(normalMapName.C_Str()))
			{
				currentMaterial.m_normalMap.isEmbedded = true;
				currentMaterial.m_normalMap.texel = reinterpret_cast<unsigned char*>(texture->pcData);
				currentMaterial.m_normalMap.width = texture->mWidth;
				currentMaterial.m_normalMap.height = texture->mHeight;
			}
			currentMaterial.m_normalMap.name = (m_directory + '/' + normalMapName.C_Str());
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------

void Resources::Model::LoadResource()
{
	if (m_loaded)
		return;

	for (Mesh& currentMesh : m_meshs)
		currentMesh.LoadVAO();

	m_loaded = true;
}

void Resources::Model::UnloadResource()
{
	if (!m_loaded)
		return;

	for (Mesh& currentMesh : m_meshs)
		currentMesh.UnloadVAO();

	m_loaded = false;
}

#ifdef MODE_EDITOR
void Resources::Model::ShowEditorControl()
{
	for (unsigned int i = 0u; i < m_meshs.size(); i++)
	{
		Mesh& currentMesh = m_meshs[i];
		Mat4x4& currentMatrix = m_defaultLocalMatrix[i];
	}

	if (ImGui::TreeNode("Material##Model"))
	{
		for (std::pair<const std::string, DefaultMaterial>& currentMaterial : m_defaultMaterial)
		{
			if (ImGui::TreeNode((currentMaterial.first + "##Model").c_str()))
			{
				ImGui::ColorEdit4(("ambient##" + currentMaterial.first + "_Model").c_str(), (float*)&currentMaterial.second.m_ambient.color);
				ImGui::Text("ambient :");
				ImGui::SameLine();
				ImGui::InputText(("##ambientTexture_" + currentMaterial.first + "_Model").c_str(), &currentMaterial.second.m_ambient.name);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						currentMaterial.second.m_ambient.name = std::string((const char*)payload->Data);

					ImGui::EndDragDropTarget();
				}
				ImGui::Spacing();

				ImGui::ColorEdit4(("diffuse##" + currentMaterial.first + "_Model").c_str(), (float*)&currentMaterial.second.m_diffuse.color);
				ImGui::Text("diffuse :");
				ImGui::SameLine();
				ImGui::InputText(("##diffuseTexture_" + currentMaterial.first + "_Model").c_str(), &currentMaterial.second.m_diffuse.name);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						currentMaterial.second.m_diffuse.name = std::string((const char*)payload->Data);

					ImGui::EndDragDropTarget();
				}
				ImGui::Spacing();

				ImGui::ColorEdit4(("specular##" + currentMaterial.first + "_Model").c_str(), (float*)&currentMaterial.second.m_specular.color);
				ImGui::Text("specular :");
				ImGui::SameLine();
				ImGui::InputText(("##specularTexture_" + currentMaterial.first + "_Model").c_str(), &currentMaterial.second.m_specular.name);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						currentMaterial.second.m_specular.name = std::string((const char*)payload->Data);

					ImGui::EndDragDropTarget();
				}
				ImGui::Spacing();

				ImGui::ColorEdit4(("emissive##" + currentMaterial.first + "_Model").c_str(), (float*)&currentMaterial.second.m_emissive.color);
				ImGui::Text("emissive :");
				ImGui::SameLine();
				ImGui::InputText(("##emissiveTexture_" + currentMaterial.first + "_Model").c_str(), &currentMaterial.second.m_emissive.name);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						currentMaterial.second.m_emissive.name = std::string((const char*)payload->Data);

					ImGui::EndDragDropTarget();
				}
				ImGui::Spacing();

				ImGui::TreePop();

				ImGui::Text("normalMap :");
				ImGui::InputText(("##normalMapTexture_" + currentMaterial.first + "_Model").c_str(), &currentMaterial.second.m_normalMap.name);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
						currentMaterial.second.m_normalMap.name = std::string((const char*)payload->Data);

					ImGui::EndDragDropTarget();
				}
				ImGui::Spacing();

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}
#endif // MODE_EDITOR
