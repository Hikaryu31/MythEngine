#pragma once

#include "define.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <typeinfo>
#include <functional>

#include "tinyxml2.h"
#include "PhysX/PxPhysicsAPI.h"

#include "ECS/Component.h"
#include "ECS/Transform.h"

// m_components.emplace_back<std::unique_ptr<ECS::Component>>(p_component)

namespace Core
{
	class GameObject
	{
	public:
		GameObject(const std::string& p_name) : m_name(p_name), m_deleteFlag(false), m_components() {}
		~GameObject() {}

		std::string&       GetName() { return m_name; }
		const std::string& GetName() const { return m_name; }
		bool HasDeleteFlag() const { return m_deleteFlag; }
		void SetDeleteFlag(bool p_value = true) { m_deleteFlag = p_value; }

		std::vector<std::unique_ptr<ECS::Component>>&       GetComponents() { return m_components; }
		const std::vector<std::unique_ptr<ECS::Component>>& GetComponents() const { return m_components; }

		template <class T>
		T* GetComponent(const unsigned int indexComponent = 0u);
		template <class T>
		bool TryGetComponent(T** out_component, const unsigned int indexComponent = 0u);
		template <class T>
		std::vector<T*> GetAllComponents();

		void Start();
		void Update();
		void FixedUpdate();
		void setTransform(physx::PxTransform p_newTransform);
#ifdef MODE_EDITOR
		void EditorUpdate();
		void EditorFixedUpdate();
		void ShowEditorControl();

		unsigned int GetId() const { return m_ID; }

		static void InitializeComponentType();
		static void AddComponentType(const char* p_className, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback);

		static void SetupGameObjectID(std::vector<std::unique_ptr<GameObject>>& m_gameObjectsList);
#endif // MODE_EDITOR
		void Draw();
		//void DrawUi();

		void RegisterUiComponent(std::map<float, std::pair<GameObject*, ECS::UiRenderComponent*>>& p_uiComponent);

	private:
		std::string m_name;
		bool        m_deleteFlag;

		std::vector<std::unique_ptr<ECS::Component>> m_components;

#ifdef MODE_EDITOR
		unsigned int m_ID = 0u;

		static std::vector<const char*> m_componentName;
		static std::vector<std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> m_gComponentLoader;
#endif // MODE_EDITOR
	};

	template<class T>
	inline T* GameObject::GetComponent(const unsigned int indexComponent)
	{
		T* toReturnPtr = nullptr;
		unsigned int currentComponent = 0;

		for (unsigned int i = 0; i < m_components.size(); i++)
		{
			if (typeid(*m_components[i]) == typeid(T))
			{
				toReturnPtr = dynamic_cast<T*>(m_components[i].get());

				if (currentComponent < indexComponent)
					currentComponent++;
				else
					return toReturnPtr;
			}
		}

		return toReturnPtr;
	}

	template<class T>
	inline bool GameObject::TryGetComponent(T** out_component, const unsigned int indexComponent)
	{
		T* toTestReturn = GetComponent<T>(indexComponent);

		if (toTestReturn == nullptr)
			return false;

		if (out_component != nullptr)
			*out_component = toTestReturn;

		return true;
	}

	template<class T>
	inline std::vector<T*> GameObject::GetAllComponents()
	{
		std::vector<T*> toReturnlist;
		unsigned int currentComponent = 0;

		for (unsigned int i = 0; i < m_components.size(); i++)
		{
			if (typeid(*m_components[i]) == typeid(T))
				toReturnlist.push_back(dynamic_cast<T*>(m_components[i].get()));
		}

		return toReturnlist;
	}
}
