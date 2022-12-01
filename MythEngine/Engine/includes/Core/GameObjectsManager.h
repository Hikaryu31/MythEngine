#pragma once

#include "define.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "GameObject.h"
#include "Resources/Scene.h"
#include "ECS/HDR.h"

// m_currentScene.emplace_back<std::unique_ptr<Core::GameObject>>(p_gameObject)

namespace Core
{
	class GameObjectManager
	{
	public:
		GameObjectManager() : m_gameObjects(), m_currentScene(nullptr) {}
		~GameObjectManager() {}

		void LoadScene(std::shared_ptr<Resources::Scene> p_newScene, bool p_isNotReload = true);
		std::shared_ptr<Resources::Scene> GetCurrentScene() const { return m_currentScene; }
		bool HasScene() const { return m_currentScene != nullptr; }

		std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_gameObjects; }
		const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

		void Start();
		void Update();
		void FixedUpdate();
#ifdef MODE_EDITOR
		void EditorUpdate();
		void EditorFixedUpdate();
		void ShowManagerWindow(bool* p_opened = nullptr);
		void ShowEditorWindow(bool* p_opened = nullptr);
#endif // MODE_EDITOR
		void Draw();
		void DrawUi();

	private:
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;
		std::shared_ptr<Resources::Scene> m_currentScene;
		std::unique_ptr<ECS::HDR> hdr;

		//Ui special needs
		std::map<float, std::pair<GameObject*, ECS::UiRenderComponent*>> m_uiRenderComponent;

#ifdef MODE_EDITOR
		unsigned int m_selectedGameObject = 0u;
#endif // MODE_EDITOR
	};
}