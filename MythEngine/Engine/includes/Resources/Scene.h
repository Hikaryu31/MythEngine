#pragma once

#include "define.h"

#include "Core/GameObject.h"
#include "ECS/Component.h"
#include "Resource.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Resources
{
	class ResourcesManager;
}

namespace Resources
{
	class Scene
	{
	public:
		Scene(const std::string& p_name) : m_resourceManager(nullptr), m_name(p_name) {}
		virtual ~Scene() {}

		void SetResourcesManager(Resources::ResourcesManager* p_newRM = nullptr) { m_resourceManager = p_newRM; }

		const std::string& GetName() const { return m_name; }

		virtual void RegisterGameObjects(std::vector<std::unique_ptr<Core::GameObject>>& p_GameObjectList) = 0;

#ifdef MODE_EDITOR
		// only in editor mode (only when MODE_EDITOR is define)
		virtual bool HasSaveSupport() const { return false; }
		// only in editor mode (only when MODE_EDITOR is define)
		virtual void SaveGameObjects(/*const std::unordered_map<std::string, std::shared_ptr<Resource>>& p_resourcesList,*/const std::vector<std::unique_ptr<Core::GameObject>>& p_gameObjectList) {}
#endif // MODE_EDITOR

	protected:
		Resources::ResourcesManager* m_resourceManager;

	private:
		std::string m_name;
	};

	class BasicScene : public Scene
	{
	public:
		BasicScene(const std::string& p_name) : Scene(p_name) {}
		~BasicScene() {}

		void RegisterGameObjects(std::vector<std::unique_ptr<Core::GameObject>>& p_GameObjectList) override;

#ifdef MODE_EDITOR
		// only in editor mode (only when MODE_EDITOR is define)
		bool HasSaveSupport() const override { return true; }
		// only in editor mode (only when MODE_EDITOR is define)
		void SaveGameObjects(/*const std::unordered_map<std::string, std::shared_ptr<Resource>>& p_resourcesList,*/const std::vector<std::unique_ptr<Core::GameObject>>& p_gameObjectList) override;
#endif // MODE_EDITOR

		//static void AddComponentType(const size_t& p_classHash, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback);
		static void AddComponentType(const char* p_className, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback);
		static std::string GlobalScenePath;

		//static std::unordered_map<size_t, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> m_sComponentLoader;
		static std::unordered_map<std::string, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> m_sComponentLoader;
	private:
	};

#ifdef MODE_EDITOR
	class TemporaryScene : public Scene
	{
	public:
		TemporaryScene() : Scene("temp") {}
		~TemporaryScene() {}

		void RegisterGameObjects(std::vector<std::unique_ptr<Core::GameObject>>& p_GameObjectList);
		void SaveGameObjects(const std::vector<std::unique_ptr<Core::GameObject>>& p_gameObjectList);

	private:
		std::string m_data;
	};
#endif // MODE_EDITOR
}