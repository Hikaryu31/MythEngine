#pragma once

#include "define.h"

#include "tinyxml2.h"

#define REGISTER_COMPONENT_HPP(CompType) static const char* TypeName; \
	virtual const char* GetTypeName() const override { return TypeName; } \
	virtual ECS::COMPONENT_TYPE ComponentType() const override { return CompType; }\
	static void Create(std::vector<std::unique_ptr<ECS::Component>>& lstComponent);
#define REGISTER_COMPONENT_CPP(CurComponent) const char* CurComponent::TypeName = #CurComponent;\
	void CurComponent::Create(std::vector<std::unique_ptr<ECS::Component>>& lstComponent) { lstComponent.emplace_back(std::make_unique<CurComponent>()); }

namespace Core
{
	class GameObject;
}

namespace ECS
{
	enum COMPONENT_TYPE_ : unsigned char
	{
		COMPONENT_TYPE_NOTHING = 0,
		COMPONENT_TYPE_GAMEPLAY = 1 << 0,
		COMPONENT_TYPE_PHYSIC = 1 << 1,
		COMPONENT_TYPE_RENDER = 1 << 2,
		COMPONENT_TYPE_UI_RENDER = 1 << 3
	};
	typedef unsigned char COMPONENT_TYPE;

	class Component
	{
	public:
		Component() : m_enabled(true), m_deleteFlag(false) {}
		virtual ~Component() {}

		virtual const char* GetTypeName() const = 0;
		virtual COMPONENT_TYPE ComponentType() const = 0;

		bool Enabled() const { return m_enabled; }
		void SetEnabled(bool p_value = true) { m_enabled = p_value; }
		bool HasDeleteFlag() const { return m_deleteFlag; }
		void SetDeleteFlag(bool p_value = true) { m_deleteFlag = p_value; }

		virtual void Start(Core::GameObject* p_gameObject) {}
		virtual void Load(tinyxml2::XMLElement* ComponentXml) {}
		virtual void Load(tinyxml2::XMLElement* ComponentXml, Core::GameObject* p_gameObject) {}
#ifdef MODE_EDITOR
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		virtual void ShowEditorControl(const unsigned int p_indexComponent) {}
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		virtual void Save(tinyxml2::XMLElement* ComponentXml) {}
#endif // MODE_EDITOR

	private:
		bool m_enabled;
		bool m_deleteFlag;
	};

	class GameplayComponent
	{
	public:
		GameplayComponent() {}
		virtual ~GameplayComponent() {}

		virtual void Update(Core::GameObject* p_gameObject) = 0;
#ifdef MODE_EDITOR
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		virtual void EditorUpdate(Core::GameObject* p_gameObject) {}
#endif // MODE_EDITOR
	private:
	};

	class PhysicComponent
	{
	public:
		PhysicComponent() {}
		virtual ~PhysicComponent() {}

		virtual void FixedUpdate(Core::GameObject* p_gameObject) = 0;
#ifdef MODE_EDITOR
		// don't forget preprocessor : #ifdef MODE_EDITOR and #endif
		virtual void EditorFixedUpdate(Core::GameObject* p_gameObject) {}
#endif // MODE_EDITOR
	private:
	};

	class RenderComponent
	{
	public:
		RenderComponent() {}
		virtual ~RenderComponent() {}

		virtual void Draw(Core::GameObject* p_gameObject) = 0;
	private:
	};

	class UiRenderComponent
	{
	public:
		UiRenderComponent() {}
		virtual ~UiRenderComponent() {}

		virtual void DrawUi(Core::GameObject* p_gameObject) = 0;
		virtual float GetLayer() const { return 0.f; }
	private:
	};
}
