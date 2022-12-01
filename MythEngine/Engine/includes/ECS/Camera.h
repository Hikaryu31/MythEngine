#pragma once

#include "define.h"

#include "Component.h"

#include "maths/Vec3.hpp"
#include "maths/Quaternion.hpp"

#include "Transform.h"

#include <vector>
#include <memory>
#include <event_notifier.hpp>

namespace ECS
{
	class Camera : public Component
	{
	public:
		Camera(const bool p_isOrthographic = false);
		~Camera();

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_NOTHING)

		bool IsOrthographic() const { return m_isOrthographic; }
		void SetOrthographic(const bool p_isOrthographic) { m_isOrthographic = p_isOrthographic; }

		bool IsSelected() const { return m_isSelected; }
		void SetSelected(const bool p_isSelected) { m_isSelected = p_isSelected; }

		Vec3       Position;
		Vec3       Offset = (0.f,0.f,0.f);
		Quaternion Rotation;
		//float Pitch = 0.f;
		//float Yaw = 0.f;
		unsigned int IndexTransform = 0u;

		Mat4x4 GetViewMatrix() const;

		void Start(Core::GameObject* p_gameObject) override;
		void TransformUpdate(ECS::Transform* p_transform);
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

	private:
		bool m_isOrthographic;
		bool m_isSelected;

		stypox::EventNotifier::Handler m_updateEventHandler;
	};
}