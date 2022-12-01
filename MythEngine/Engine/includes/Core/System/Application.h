 #pragma once

#include "define.h"

#include "Core/BaseSingleton.h"

#include <PhysX/PxPhysicsAPI.h>
#include "Core/System/PhysicSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <miniaudio.h>

#include "imgui.h"
#include "imgui_stdlib.h"

#include "Resources/ResourcesManager.h"
#include "Core/GameObjectsManager.h"
#include "Core/System/Input.h"
#include "Core/System/Time.h"
#include "Core/Debug/Log.h"
#include "Core/Debug/Draw.h"
#include "Core/System/CameraManager.h"
#include "Core/System/LightManager.h"
#include "Core/System/EventManager.h"
#include "Resources/Scene.h"

namespace Core
{
	namespace System
	{
		class Application : public BaseSingleton<Application>
		{
		public:
			Application();
			~Application() { singleton = nullptr; }

			PhysicSystem* m_physicSystem = nullptr;

			const std::string& GetName() const { return m_name; }

			Resources::ResourcesManager& GetResourcesManager() { return m_resourcesManager; }
			const Resources::ResourcesManager& GetResourcesManager() const { return m_resourcesManager; }
			GameObjectManager& GetGameObjectManager() { return m_gameObjectManager; }
			const GameObjectManager& GetGameObjectManager() const { return m_gameObjectManager; }
			ma_engine& GetAudioEngine() { return m_audioEngine; }
			const ma_engine& GetAudioEngine() const { return m_audioEngine; }

			void LoadSettings();
#ifdef MODE_EDITOR
			void LoadEditorSettings();
			void ShowSettingsWindow(bool* p_opened);
			void SaveSettings();
			void SaveEditorSettings();
			bool PlayAnimation() const { return m_playAnimation; }
#else
			bool PlayAnimation() const { return true; }
#endif // MODE_EDITOR
			static void QueryLoadScene(const std::string& p_nextSceneName) { MYTH_ASSERT(singleton != nullptr); singleton->m_queryLoadScene = true; singleton->m_nextSceneName = p_nextSceneName; }

			static bool Initialization(int p_width, int p_height, const std::string& p_name, bool p_fullscreen = false);
			static void ResetPhysicsSytem();
			static bool ShouldExit() { MYTH_ASSERT(singleton != nullptr); return (singleton->m_glfwWindow == nullptr ? true : glfwWindowShouldClose(singleton->m_glfwWindow) || singleton->m_shouldExit); }
			static void UpdateAndDraw();
			static void Kill();

		private:
			std::string m_name = "none";
			int         m_width = 100;
			int         m_height = 100;
			GLFWwindow* m_glfwWindow = nullptr;
			bool        m_shouldExit = false;

			GameObjectManager m_gameObjectManager;
			Resources::ResourcesManager m_resourcesManager;
			ma_engine m_audioEngine;

			Core::System::Time m_time;
			Core::System::Input m_input;
			Core::System::CameraManager m_cameraManager;
			Core::System::LightManager m_lightManager;
			Core::System::EventManager m_eventManager;
			Core::Debug::Draw m_drawDebug;
			Core::Debug::Log m_log;

			// change scene
			bool        m_queryLoadScene = false;
			std::string m_nextSceneName = "";

#ifdef MODE_EDITOR
			bool m_editorOpen = true;

			bool m_demoImGuiWindow = false;
			bool m_settingsWindow = true;
			bool m_consoleWindow = true;
			bool m_gameObjectManagerWindow = true;
			bool m_gameObjectEditorrWindow = true;
			bool m_resourcesManagerWindow = true;
			bool m_resourcesSceneWindow = true;
			bool m_resourceEditorWindow = true;
			bool m_inputEditorWindow = true;

			bool m_run = false;
			std::shared_ptr<Resources::TemporaryScene> m_tempScene = nullptr;

			// ---- Settings ----

			bool m_hasSceneOnLoading = false;
			std::string m_sceneOnLoading = "";

			bool m_playAnimation = true;
#endif // MODE_EDITOR
		};
	}
}
