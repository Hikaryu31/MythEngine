#pragma once

#include "define.h"

#include "Component.h"

#include "maths/Maths.hpp"
#include "maths/Vec3.hpp"
#include "maths/Quaternion.hpp"
#include "maths/Mat4x4.hpp"

#include "ImGuizmo.h"

#include "event_notifier.hpp"

#include <vector>
#include <memory>

namespace ECS
{
	class Transform : public Component, public PhysicComponent
	{
	public:
		Transform(Transform* p_parent = nullptr);
		~Transform();

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_PHYSIC)

		Transform* GetParent() const { return m_parent; }
		bool HasParent() const { return (m_parent != nullptr); }
		void SetParent(Transform* p_parent, bool unsetParent = true);
		void AddChild(Transform* p_child);
		void RemChild(Transform* p_child);

		Vec3&       Position() { m_update = true; return m_position; }
		Quaternion& Rotation() { m_update = true; return m_rotation; }
		Vec3&       Scale() { m_update = true; return m_scale; }

		const Vec3&       GetPosition() const { return m_position; }
		const Quaternion& GetRotation() const { return m_rotation; }
		const Vec3&       GetScale() const { return m_scale; }

		void	SetPosition(Vec3 p_newPosition) { m_updatePhysicsOnUpdate = false; m_update = true; m_position = p_newPosition; }
		void	SetRotation(Quaternion p_newRotation) { m_updatePhysicsOnUpdate = false; m_update = true; m_rotation = p_newRotation; }
		void	SetScale(Vec3 p_newScale) { m_updatePhysicsOnUpdate = false; m_update = true; m_scale = p_newScale; }

		Vec3          GetGlobalPosition() const;
		Quaternion    GetGlobalRotation() const;
		Vec3          GetGlobalScale() const;
		const Mat4x4& GetGlobalMatrix() const { return m_globalMatrix; }

		stypox::EventNotifier& GetUpdateEvent() { return *m_updateEvent; }

		void Start(Core::GameObject* p_gameObject) override;
		void FixedUpdate(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml, Core::GameObject* p_gameObject) override;
#ifdef MODE_EDITOR
		void EditorFixedUpdate(Core::GameObject* p_gameObject) override;
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

		bool m_update;
		bool m_updatePhysicsOnUpdate = true;
		bool updatePhysicsTransform = true;
	private:
		Transform* m_parent;
		std::vector<Transform*> m_childs;

		Vec3       m_position;
		Quaternion m_rotation;
		Vec3       m_scale;
		//bool       m_update;
		Mat4x4     m_localMatrix;
		Mat4x4     m_globalMatrix;

		std::shared_ptr<stypox::EventNotifier> m_updateEvent = nullptr;

#ifdef MODE_EDITOR
		static std::vector<std::pair<std::string, Transform*>> listGlobalTransform;
		static int item_current_idx;

		ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		Vec3 m_editorRotation = Vec3::Zero;
		Mat4x4 mCurrentGuizmoMatrix = Mat4x4::Identity;

		void UpdateTransform(bool updateEditorRotation = true);
#else
		void UpdateTransform();
#endif // MODE_EDITOR
	};
}