#pragma once
#include "Core/System/Application.h"
#include "ECS/Component.h"

enum Shape
{
	BOX,
	CAPSULE,
	SPHERE
};

namespace ECS
{
#ifdef MODE_EDITOR
	class RigidBody : public Component, public GameplayComponent, public RenderComponent
#else
	class RigidBody : public Component, public GameplayComponent
#endif
	{
	public:
		RigidBody();
		~RigidBody();

#ifdef MODE_EDITOR
		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY | COMPONENT_TYPE_RENDER)
#else
		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)
#endif
		void Load(tinyxml2::XMLElement* ComponentXml) override;

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void UpdateColliderPos(Core::GameObject* p_gameObject);
		void UpdateOffset();

		physx::PxMaterial* m_material = nullptr;

		bool m_newOffset = true;

		bool m_isKinematic = false;
		bool m_isTrigger = false;
		bool m_useGravity = true;

		bool m_lockTranslationAxisX = false;
		bool m_lockTranslationAxisY = false;
		bool m_lockTranslationAxisZ = false;

		bool m_lockRotationAxisX = false;
		bool m_lockRotationAxisY = false;
		bool m_lockRotationAxisZ = false;

		physx::PxTransform m_colliderLocalTr;
		physx::PxRigidDynamic* m_actor;

		unsigned int m_shape = Shape::BOX;
		Vec3 m_colliderOffset = Vec3::Zero;

		Vec3 m_boxScale = {.5f, .5f, .5f};

		float m_capsuleHalfHeight = .5f;
		float m_capsuleRadius = .5f;

		float m_sphereRadius = .5f;
#ifdef MODE_EDITOR
		void Draw(Core::GameObject* currentGo) override;
		void EditorUpdate(Core::GameObject* p_gameObject) override;
		void ShowEditorControl(const unsigned int p_indexComponent) override;

		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR

	private:
		void SetCollider();
		void SetConstraints();
		void SetKinematic();
		void SetTrigger();
		void SetUsingGravity();

		//Display controls fot gravity, trigger, kinematic, draw
		void ShowBooleans(const unsigned int p_indexComponent);

		//Display current collider linear and angular constraints
		void ShowConstraints(const unsigned int p_indexComponent);

		//Display collider shape controller
		void ShowShapeOptions(const unsigned int p_indexComponent);

		//Display different size sliders according to collider shape
		void ShowShapeScales(const unsigned int p_indexComponent);
#ifdef MODE_EDITOR
		bool m_drawCollider = false;
#endif // MODE_EDITOR
	};
}
