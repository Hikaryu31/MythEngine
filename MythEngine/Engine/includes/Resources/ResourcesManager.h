#pragma once

#include "define.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <typeinfo>

#include "Scene.h"

#include "tinyxml2.h"

namespace Resources
{
	class ResourcesManager
	{
	public:
		ResourcesManager() : m_resources(), m_scenes() {}
		~ResourcesManager() {}

		template <class T>
		std::vector<std::shared_ptr<T>> GetAllResources();
		/*template <class T>
		std::shared_ptr<T> GetResource(const std::string& p_resourceName);
		template <class T>
		bool               TryGetResource(const std::string& p_resourceName, std::shared_ptr<T>* outResource);/**/
		template <class T>
		std::shared_ptr<T> GetAndLoadResource(const std::string& p_resourceName);
		template <class T>
		bool               TryGetAndLoadResource(const std::string& p_resourceName, std::shared_ptr<T>* outResource);
		bool               RegisterAndLoadResource(std::shared_ptr<Resource> p_newResource);
		bool               RegisterAndLoadResource(Resource* p_newResource);
		void               UnloadUnusedResource();

		std::shared_ptr<Scene> GetScene(const std::string& p_sceneName);
		bool                   AddScene(std::shared_ptr<Scene>& p_newScene);

#ifdef MODE_EDITOR
		void ShowManagerWindow(bool* p_opened = nullptr);
		void ShowSceneWindow(bool* p_opened = nullptr);
		void ShowEditorWindow(bool* p_opened = nullptr);
		void SaveScene(tinyxml2::XMLElement* SceneListXml);
#endif // MODE_EDITOR

 	private:
		std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;

#ifdef MODE_EDITOR
		std::string m_selected = "";
		std::vector<std::pair<std::string, bool>> m_resourcesFilter;
#endif // MODE_EDITOR
	};

	template<class T>
	inline std::vector<std::shared_ptr<T>> ResourcesManager::GetAllResources()
	{
		std::vector<std::shared_ptr<T>> to_return = std::vector<std::shared_ptr<T>>();

		for (std::pair<const std::string, std::shared_ptr<Resource>>& currentResource : m_resources)
			if (typeid(*currentResource.second) == typeid(T))
				to_return.push_back(std::dynamic_pointer_cast<T, Resource>(currentResource.second));

		return to_return;
	}

	/*template<class T>
	inline std::shared_ptr<T> ResourcesManager::GetResource(const std::string& p_resourceName)
	{
		if (m_resources.find(p_resourceName) == m_resources.end())
			return nullptr;
		if (typeid(*m_resources[p_resourceName]) != typeid(T))
			return nullptr;

		return std::dynamic_pointer_cast<T, Resource>(m_resources[p_resourceName]);
	}
	template<class T>
	inline bool ResourcesManager::TryGetResource(const std::string& p_resourceName, std::shared_ptr<T>* outResource)
	{
		std::shared_ptr<T> toTestReturn = GetResource<T>(p_resourceName);

		if (toTestReturn == nullptr)
			return false;

		if (outResource != nullptr)
			*outResource = toTestReturn;

		return true;
	}/**/

	template<class T>
	inline std::shared_ptr<T> ResourcesManager::GetAndLoadResource(const std::string& p_resourceName)
	{
		if (m_resources.find(p_resourceName) == m_resources.end())
			return nullptr;
		if (typeid(*m_resources[p_resourceName]) != typeid(T))
			return nullptr;

		m_resources[p_resourceName]->LoadResource();
		return std::dynamic_pointer_cast<T, Resource>(m_resources[p_resourceName]);
	}
	template<class T>
	inline bool ResourcesManager::TryGetAndLoadResource(const std::string& p_resourceName, std::shared_ptr<T>* outResource)
	{
		std::shared_ptr<T> toTestReturn = GetAndLoadResource<T>(p_resourceName);

		if (toTestReturn == nullptr)
			return false;

		if (outResource != nullptr)
			*outResource = toTestReturn;

		return true;
	}
}
