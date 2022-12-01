#pragma once

#include "define.h"

#include "Component.h"

#include "Transform.h"
#include "maths/Vec3.hpp"

#include <vector>
#include <memory>

namespace ECS
{
	class PointLight : public Component, public GameplayComponent
	{
	public:
		PointLight();
		~PointLight();

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)

		Vec3 Position = Vec3::Zero;

		float Constant = 0.f;
		float Linear = 0.f;
		float Quadratic = 0.f;

		Vec3 Ambient = Vec3(0.1f, 0.1f, 0.1f);
		Vec3 Diffuse = Vec3::One;
		Vec3 Specular = Vec3::One;

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { Update(p_gameObject); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR
	
	private:
	};

	class DirectionLight : public Component, public GameplayComponent
	{
	public:
		DirectionLight() : Component() {}
		~DirectionLight();

		bool IsSelected() const { return m_selected; }
		void SetSelected(const bool p_isSelected) { m_selected = p_isSelected; }

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)

		Vec3 Direction = Vec3::Forward;

		Vec3 Ambient = Vec3(0.1f, 0.1f, 0.1f);
		Vec3 Diffuse = Vec3::One;
		Vec3 Specular = Vec3::One;

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { Update(p_gameObject); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

	private:
		bool m_selected = false;

#ifdef MODE_EDITOR
		Vec3 m_editorRotation = Vec3::Zero;
#endif // MODE_EDITOR

	};
}