#pragma once

#include "define.h"

#include "ECS/Component.h"

#include "Resources/Model.h"
#include "Resources/Texture.h"

#include <memory>
#include <unordered_map>

namespace ECS
{
	struct Material
	{
		float m_shininess = 1.f;

		Vec4 m_ambientColor = { 0.f, 0.f, 0.f, 1.f };
		Vec4 m_diffuseColor = { 0.f, 0.f, 0.f, 1.f };
		Vec4 m_specularColor = { 0.f, 0.f, 0.f, 1.f };
		Vec4 m_emissiveColor = { 0.f, 0.f, 0.f, 1.f };
		std::shared_ptr<Resources::Texture> m_ambientTexture = nullptr;
		std::shared_ptr<Resources::Texture> m_diffuseTexture = nullptr;
		std::shared_ptr<Resources::Texture> m_specularTexture = nullptr;
		std::shared_ptr<Resources::Texture> m_emissiveTexture = nullptr;
		std::shared_ptr<Resources::Texture> m_normalMap = nullptr;
	};

	class ModelInstance : public Component, public RenderComponent, public GameplayComponent
	{
	public:
		ModelInstance();
		~ModelInstance() {}

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_RENDER | COMPONENT_TYPE_GAMEPLAY)
		
		std::shared_ptr<Resources::Model> Model = nullptr;
		std::shared_ptr<Resources::Shader> Shader = nullptr;
		bool UseLight = true;
		std::vector<Mat4x4> m_boneTransforms;
		bool m_hasAnim = false;
		unsigned int IndexTransform = 0u;

		std::unordered_map<std::string, Material>& GetMaterials() { return m_materials; }
		std::vector<Mat4x4>& GetLocalMatrix() { return m_localMatrix; }
		Mat4x4& GetWorldMatrix() { return m_worldMatrix; }

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { Update(p_gameObject); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR
		void Draw(Core::GameObject* p_gameObject) override;

	private:
		std::unordered_map<std::string, Material> m_materials;
		std::vector<Mat4x4> m_localMatrix;
		Mat4x4 m_worldMatrix = Mat4x4::Identity;

#ifdef MODE_EDITOR
		void ModelCreator();
		void ShaderCreator();
#endif // MODE_EDITOR

		void DuplicatePart();
	};
}
