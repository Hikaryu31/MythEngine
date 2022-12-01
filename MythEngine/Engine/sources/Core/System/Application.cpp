#include "Core/System/Application.h"

#include "Core/System/Random.h"

#include "ECS/FreeFlyMovement.h"
#include "ECS/Text.h"
#include "ECS/ModelInstance.h"
#include "ECS/Animator.h"
#include "ECS/RigidBody.h"
#include "ECS/Audio.h"

#include "tinyxml2.h"
#include "Core/System/XMLMacro.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "ImGuizmo.h"

#ifdef MODE_EDITOR
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#endif // EDITOR_MODE

SINGLETON_SETUP(Core::System::Application)

Core::System::Application::Application()
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;

#ifdef MODE_EDITOR
	m_tempScene = std::make_shared<Resources::TemporaryScene>();
#endif // MODE_EDITOR
}

void Core::System::Application::LoadSettings()
{
    MYTH_DEBUG("LoadSettings() called !!");

    tinyxml2::XMLDocument currentSceneDocument;
    if (currentSceneDocument.LoadFile("resources/settings.xml") != tinyxml2::XML_SUCCESS)
        return;

    tinyxml2::XMLElement* inputs = nullptr;
    if (XMLMacro::GetElement(currentSceneDocument, &inputs, "inputs"))
    {
        std::string InputName = "";

        for (tinyxml2::XMLElement* input = inputs->FirstChildElement("input"); input != nullptr; input = input->NextSiblingElement())
        {
            InputName = input->Attribute("name");

            for (tinyxml2::XMLElement* key = input->FirstChildElement("key"); key != nullptr; key = key->NextSiblingElement())
				m_input.AddKeyInput(InputName, key->IntAttribute("code"), key->BoolAttribute("mouse"));
		}
	}

	tinyxml2::XMLElement* scenes = nullptr;
	if (XMLMacro::GetElement(currentSceneDocument, &scenes, "scenes"))
	{
		std::shared_ptr<Resources::Scene> scnToRegister = nullptr;

		for (tinyxml2::XMLElement* scene = scenes->FirstChildElement("scene"); scene != nullptr; scene = scene->NextSiblingElement())
		{
			const char* currentSceneName = scene->Attribute("name");

			if (m_resourcesManager.GetScene(currentSceneName) != nullptr)
				continue;

			scnToRegister = std::static_pointer_cast<Resources::Scene, Resources::BasicScene>(std::make_shared<Resources::BasicScene>(currentSceneName));
			m_resourcesManager.AddScene(scnToRegister);
		}

		if (scenes->FindAttribute("load_on_start") != nullptr)
		{
#ifdef MODE_EDITOR
			m_hasSceneOnLoading = true;
			m_sceneOnLoading = scenes->Attribute("load_on_start");
			QueryLoadScene(m_sceneOnLoading);
#else
			QueryLoadScene(scenes->Attribute("load_on_start"));
#endif // MODE_EDITOR
		}
	}
}

#ifdef MODE_EDITOR
void Core::System::Application::LoadEditorSettings()
{
	MYTH_DEBUG("LoadEditorSettings() called !!");

	tinyxml2::XMLDocument currentSceneDocument;
	if (currentSceneDocument.LoadFile("editorsettings.xml") != tinyxml2::XML_SUCCESS)
	{
		m_input.AddKeyInput(EDITOR_INPUT_NAME, 96);
		return;
	}

	bool NotAddEditorInput = true;

	tinyxml2::XMLElement* settingsXML = nullptr;
	if (XMLMacro::GetElement(currentSceneDocument, &settingsXML, "settings"))
	{
		tinyxml2::XMLElement* editorKeyXML = nullptr;
		if (XMLMacro::GetElement(settingsXML, &editorKeyXML, "editorkey"))
		{
			m_input.AddKeyInput(EDITOR_INPUT_NAME, editorKeyXML->IntText(), editorKeyXML->BoolAttribute("mouse"));
			NotAddEditorInput = false;
		}

		tinyxml2::XMLElement* play_animation = nullptr;
		if (XMLMacro::GetElement(settingsXML, &play_animation, "play_animation"))
			m_playAnimation = play_animation->BoolText();
	}

	if (NotAddEditorInput)
		m_input.AddKeyInput(EDITOR_INPUT_NAME, 96);
}

void Core::System::Application::ShowSettingsWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("Settings", p_opened);

	ImGui::Checkbox("##m_hasSceneOnLoading", &m_hasSceneOnLoading);
	ImGui::SameLine();
	ImGui::InputText("Start scene##OnLoading", &m_sceneOnLoading, (m_hasSceneOnLoading ? 0 : ImGuiInputTextFlags_ReadOnly));

	ImGui::Checkbox("Play animation", &m_playAnimation);

	ImGui::End();
}

void Core::System::Application::SaveSettings()
{
	MYTH_DEBUG("SaveSettings() called !!");

	tinyxml2::XMLDocument currentSceneDocument;
	MYTH_ASSERT(currentSceneDocument.Parse("<?xml version=\"1.0\"?><inputs></inputs><scenes></scenes>") == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* lstInputsXML = nullptr;
	if (XMLMacro::GetElement(currentSceneDocument, &lstInputsXML, "inputs"))
		m_input.SaveInputs(lstInputsXML);

	tinyxml2::XMLElement* lstSceneXML = nullptr;
	if (XMLMacro::GetElement(currentSceneDocument, &lstSceneXML, "scenes"))
	{
		m_resourcesManager.SaveScene(lstSceneXML);

		if (m_hasSceneOnLoading)
			lstSceneXML->SetAttribute("load_on_start", m_sceneOnLoading.c_str());
	}

	currentSceneDocument.SaveFile("resources/settings.xml");
}

void Core::System::Application::SaveEditorSettings()
{
	MYTH_DEBUG("SaveEditorSettings() called !!");

	tinyxml2::XMLDocument currentSceneDocument;
	MYTH_ASSERT(currentSceneDocument.Parse("<?xml version=\"1.0\"?><settings></settings>") == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* settingsXML = nullptr;
	if (XMLMacro::GetElement(currentSceneDocument, &settingsXML, "settings"))
	{
		tinyxml2::XMLElement* editorKeyXML = settingsXML->InsertNewChildElement("editorkey");
		editorKeyXML->SetText(96);

		Core::System::KeyboardMouseInputData* editorKeyData = m_input.GetKeyInput(EDITOR_INPUT_NAME);
		if (editorKeyData != nullptr)
			if (editorKeyData->KeyCodes.size() >= 1Ui64)
			{
				editorKeyXML->SetText(editorKeyData->KeyCodes[0].second);
				if (editorKeyData->KeyCodes[0].first)
					editorKeyXML->SetAttribute("mouse", true);
			}

		// ------------------------------------------------------

		tinyxml2::XMLElement* playAnimationXML = settingsXML->InsertNewChildElement("play_animation");
		playAnimationXML->SetText(m_playAnimation);
	}

	currentSceneDocument.SaveFile("editorsettings.xml");
}
#endif // MODE_EDITOR

bool Core::System::Application::Initialization(int p_width, int p_height, const std::string& p_name, bool p_fullscreen)
{
	MYTH_ASSERT(singleton != nullptr);

	const char* glsl_version = "#version 450 core";

	// Initialize the library
	if (!glfwInit())
	{
		MYTH_FATAL("GLFW initialization failed");
		return false;
	}

	singleton->m_physicSystem = new PhysicSystem();

	singleton->m_name = p_name;

	// Create a windowed mode window and its OpenGL context
	singleton->m_glfwWindow = glfwCreateWindow(p_width, p_height, singleton->m_name.c_str(), NULL, NULL);
	if (!singleton->m_glfwWindow)
	{
		glfwTerminate();
		MYTH_FATAL("Window creation failed");
		return false;
	}

	glfwMakeContextCurrent(singleton->m_glfwWindow);
	if (gladLoadGL() == 0)
	{
		glfwTerminate();
		MYTH_FATAL("gladLoaderLoadGL failed");
		return false;
	}

	// Make the window's context current
	glfwMakeContextCurrent(singleton->m_glfwWindow);

	ma_result resultAudioEngine = ma_engine_init(NULL, &singleton->m_audioEngine);
	if (resultAudioEngine != MA_SUCCESS)
		MYTH_ERROR("miniaudio failed to initialize");

	ma_engine_listener_set_world_up(&singleton->m_audioEngine, 0, 0, 1, 0);
	ma_engine_listener_set_cone(&singleton->m_audioEngine, 0, 1.6f, 2.f, 1.f);
	
#ifdef MODE_EDITOR
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(singleton->m_glfwWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	Core::System::SetKeyCallback(singleton->m_glfwWindow);

	singleton->LoadEditorSettings();

	//System::Input::GetInstance().AddKeyInput(EDITOR_INPUT_NAME, GLFW_KEY_E);

	Core::GameObject::InitializeComponentType();
	Core::GameObject::AddComponentType("TRANSFORM", ECS::Transform::Create);
	Core::GameObject::AddComponentType("CAMERA", ECS::Camera::Create);
	Core::GameObject::AddComponentType("FREEFLY", ECS::FreeFlyMovement::Create);
	Core::GameObject::AddComponentType("MODEL", ECS::ModelInstance::Create);
	Core::GameObject::AddComponentType("ANIMATOR", ECS::Animator::Create);
	Core::GameObject::AddComponentType("RIDIGBODY", ECS::RigidBody::Create);
	Core::GameObject::AddComponentType("POINT LIGHT", ECS::PointLight::Create);
	Core::GameObject::AddComponentType("DIRECTION LIGHT", ECS::DirectionLight::Create);
	Core::GameObject::AddComponentType("TEXT", ECS::Text::Create);
	Core::GameObject::AddComponentType("AUDIO", ECS::Audio::Create);
#endif // EDITOR_MODE

	Resources::BasicScene::AddComponentType(ECS::Transform::TypeName, ECS::Transform::Create);
	Resources::BasicScene::AddComponentType(ECS::Camera::TypeName, ECS::Camera::Create);
	Resources::BasicScene::AddComponentType(ECS::FreeFlyMovement::TypeName, ECS::FreeFlyMovement::Create);
	Resources::BasicScene::AddComponentType(ECS::ModelInstance::TypeName, ECS::ModelInstance::Create);
	Resources::BasicScene::AddComponentType(ECS::Animator::TypeName, ECS::Animator::Create);
	Resources::BasicScene::AddComponentType(ECS::Text::TypeName, ECS::Text::Create);
	Resources::BasicScene::AddComponentType(ECS::PointLight::TypeName, ECS::PointLight::Create);
	Resources::BasicScene::AddComponentType(ECS::DirectionLight::TypeName, ECS::DirectionLight::Create);
	Resources::BasicScene::AddComponentType(ECS::RigidBody::TypeName, ECS::RigidBody::Create);
	Resources::BasicScene::AddComponentType(ECS::Audio::TypeName, ECS::Audio::Create);

	singleton->m_cameraManager.Setup(singleton->m_glfwWindow);
	System::Random::Initialization();

	singleton->LoadSettings();

	MYTH_INFO("Initialization finidshed !");

	return true;
}

void Core::System::Application::ResetPhysicsSytem()
{
	if (singleton->m_physicSystem == nullptr)
		return;

	delete(singleton->m_physicSystem);
	singleton->m_physicSystem = new PhysicSystem();
}

void Core::System::Application::UpdateAndDraw()
{
	MYTH_ASSERT(singleton != nullptr);

	if (singleton->m_queryLoadScene)
	{
		std::shared_ptr<Resources::Scene> nextScene = singleton->m_resourcesManager.GetScene(singleton->m_nextSceneName);

		if (nextScene != nullptr)
		{
			singleton->m_gameObjectManager.LoadScene(nextScene);
			singleton->m_resourcesManager.UnloadUnusedResource();
			singleton->m_eventManager.ClearUnusedEvent();

			MYTH_INFO("Scene %s loaded !", nextScene->GetName().c_str());

			singleton->m_gameObjectManager.Start();
#ifdef MODE_EDITOR
			singleton->m_tempScene->SaveGameObjects(singleton->m_gameObjectManager.GetGameObjects());
#endif // EDITOR_MODE
		}
		else
			MYTH_ERROR("Scene %s does not exist !", singleton->m_nextSceneName.c_str());

		singleton->m_nextSceneName = "";
		singleton->m_queryLoadScene = false;
	}

	// Poll for and process events
	glfwPollEvents();
	singleton->m_time.Update();
	singleton->m_input.Update(singleton->m_glfwWindow);

#ifdef MODE_EDITOR
	if (singleton->m_run)
	{
		singleton->m_physicSystem->Update();
		singleton->m_gameObjectManager.Update();

		int FixedUpdateCount = System::Time::Fixing();
		while (FixedUpdateCount > 0)
		{
			singleton->m_gameObjectManager.FixedUpdate();
			FixedUpdateCount--;
		}
	}
	else
	{
		singleton->m_gameObjectManager.EditorUpdate();

		int FixedUpdateCount = System::Time::Fixing();
		while (FixedUpdateCount > 0)
		{
			singleton->m_gameObjectManager.EditorFixedUpdate();
			FixedUpdateCount--;
		}
	}
#else // !EDITOR_MODE
	singleton->m_physicSystem->Update();
	singleton->m_gameObjectManager.Update();

	int FixedUpdateCount = System::Time::Fixing();
	while (FixedUpdateCount > 0)
	{
		singleton->m_gameObjectManager.FixedUpdate();
		FixedUpdateCount--;
	}
#endif // EDITOR_MODE

	// Render here
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	singleton->m_gameObjectManager.Draw();

	glDisable(GL_DEPTH_TEST);
	singleton->m_gameObjectManager.DrawUi();
#ifndef MODE_EDITOR
	glEnable(GL_DEPTH_TEST);
#else
	singleton->m_drawDebug.Render();
	glEnable(GL_DEPTH_TEST);

	if (System::Input::IsKeyPressed(Core::System::Input::GetKeyInputIndex(EDITOR_INPUT_NAME)))
		singleton->m_editorOpen = !singleton->m_editorOpen;

	if (singleton->m_editorOpen)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuizmo::SetOrthographic(!singleton->m_cameraManager.CameraIsOrthographic());
		ImGuizmo::BeginFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (ImGuiDockNodeFlags_PassthruCentralNode & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("WorkSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID("EditorWorkSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "CTRL+S")) { singleton->SaveSettings(); }
				if (ImGui::MenuItem("Load", "CTRL+L"))
				{
					singleton->m_input.ClearAllInput();
					singleton->LoadEditorSettings();
					singleton->LoadSettings();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Demo ImGui", NULL, &(singleton->m_demoImGuiWindow))) {}
				if (ImGui::MenuItem("Settings", NULL, &(singleton->m_settingsWindow))) {}
				if (ImGui::MenuItem("Console", NULL, &(singleton->m_consoleWindow))) {}
				if (ImGui::MenuItem("GameObject Manager", NULL, &(singleton->m_gameObjectManagerWindow))) {}
				if (ImGui::MenuItem("GameObject Editor", NULL, &(singleton->m_gameObjectEditorrWindow))) {}
				if (ImGui::MenuItem("Resources Manager", NULL, &(singleton->m_resourcesManagerWindow))) {}
				if (ImGui::MenuItem("Resources Scene", NULL, &(singleton->m_resourcesSceneWindow))) {}
				if (ImGui::MenuItem("Resources Editor", NULL, &(singleton->m_resourceEditorWindow))) {}
				if (ImGui::MenuItem("Input Editor", NULL, &(singleton->m_inputEditorWindow))) {}
				ImGui::EndMenu();
			}

			if (singleton->m_run)
			{
				if (ImGui::MenuItem("Stop", NULL, &(singleton->m_run)))
				{
					singleton->m_gameObjectManager.LoadScene(std::dynamic_pointer_cast<Resources::Scene>(singleton->m_tempScene), false);
					singleton->m_resourcesManager.UnloadUnusedResource();
					singleton->m_eventManager.ClearUnusedEvent();

					singleton->m_gameObjectManager.Start();
				}
			}
			else
			{
				if (ImGui::MenuItem("Run", NULL, &(singleton->m_run)))
				{
					singleton->m_gameObjectManager.Start();
					singleton->m_tempScene->SaveGameObjects(singleton->m_gameObjectManager.GetGameObjects());
				}
			}

			ImGui::EndMainMenuBar();
		}

		if (singleton->m_demoImGuiWindow) ImGui::ShowDemoWindow(&singleton->m_demoImGuiWindow);

		singleton->ShowSettingsWindow(&singleton->m_settingsWindow);
		singleton->m_input.ShowEditorWindow(&singleton->m_inputEditorWindow);

		singleton->m_resourcesManager.ShowManagerWindow(&singleton->m_resourcesManagerWindow);
		singleton->m_resourcesManager.ShowSceneWindow(&singleton->m_resourcesSceneWindow);
		singleton->m_resourcesManager.ShowEditorWindow(&singleton->m_resourceEditorWindow);

		singleton->m_gameObjectManager.ShowManagerWindow(&singleton->m_gameObjectManagerWindow);
		singleton->m_gameObjectManager.ShowEditorWindow(&singleton->m_gameObjectEditorrWindow);

		singleton->m_log.ShowEditorWindow(&singleton->m_consoleWindow);

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
#endif // EDITOR_MODE

	// Swap front and back buffers
	glfwSwapBuffers(singleton->m_glfwWindow);
}

void Core::System::Application::Kill()
{
	MYTH_ASSERT(singleton != nullptr);

#ifdef MODE_EDITOR
	singleton->SaveEditorSettings();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif // EDITOR_MODE

	singleton->m_gameObjectManager.GetGameObjects().clear();

	ma_engine_uninit(&singleton->m_audioEngine);
	delete(singleton->m_physicSystem);

	glfwDestroyWindow(singleton->m_glfwWindow);
	glfwTerminate();
}
