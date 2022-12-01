#pragma once

#include "define.h"

#include "ECS/Component.h"

#include <utility>
#include <vector>
#include <memory>

namespace ECS
{
	constexpr const char* CAMERA_FRONT_INPUT_NAME = "CAMERA_foreward";
	constexpr const char* CAMERA_BACK_INPUT_NAME = "CAMERA_backward";
	constexpr const char* CAMERA_LEFT_INPUT_NAME = "CAMERA_leftstrafe";
	constexpr const char* CAMERA_RIGHT_INPUT_NAME = "CAMERA_rightstrafe";
	constexpr const char* CAMERA_UP_INPUT_NAME = "CAMERA_up";
	constexpr const char* CAMERA_DOWN_INPUT_NAME = "CAMERA_down";
	constexpr const char* CAMERA_SPEED_INPUT_NAME = "CAMERA_speed";
	constexpr const char* CAMERA_LOOK_INPUT_NAME = "CAMERA_look";
	constexpr const char* CAMERA_ROLL_INPUT_NAME = "CAMERA_roll";

	class FreeFlyMovement : public Component, public GameplayComponent
	{
	public:
		FreeFlyMovement(const unsigned int p_TrIndex = 0u, const float p_speed = 4.f, const float p_maxSpeed = 40.f);
		~FreeFlyMovement();

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)

		unsigned int IndexTransform() const { return m_indexTransform; }
		void SetIndexTransform(const unsigned int p_indexTransform) { m_indexTransform = p_indexTransform; }

		float Speed() const { return m_speed; }
		float MaxSpeed() const { return m_maxSpeed; }
		void SetSpeed(const float p_speed, const float p_maxSpeed) { m_speed = p_speed; m_maxSpeed = p_maxSpeed; }

		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override;
		void ShowEditorControl(const unsigned int p_indexComponent);
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

	private:
		unsigned int m_indexTransform;

		float m_speed;
		float m_maxSpeed;
	};
}