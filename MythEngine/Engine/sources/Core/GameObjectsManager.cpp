#include "Core/GameObjectsManager.h"
#include "maths/Mat4x4.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "Core/System/Application.h"

void Core::GameObjectManager::LoadScene(std::shared_ptr<Resources::Scene> p_newScene, bool p_isNotReload)
{
	if (p_isNotReload)
		m_currentScene = p_newScene;

	m_gameObjects.clear();

	Core::System::Application::ResetPhysicsSytem();

	if (p_isNotReload)
	{
		if (m_currentScene != nullptr)
			m_currentScene->RegisterGameObjects(m_gameObjects);
	}
	else
	{
		if (p_newScene != nullptr)
			p_newScene->RegisterGameObjects(m_gameObjects);
	}
}

void Core::GameObjectManager::Start()
{
	hdr = std::make_unique<ECS::HDR>();
	for (std::unique_ptr<GameObject>& currentGO : m_gameObjects)
	{
		currentGO->Start();
	}
}

void Core::GameObjectManager::Update()
{
	m_uiRenderComponent.clear();

	hdr->Update();

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Update();

		if (m_gameObjects[i]->HasDeleteFlag())
		{
			m_gameObjects.erase(m_gameObjects.begin() + i);
			i--;
			continue;
		}

		m_gameObjects[i]->RegisterUiComponent(m_uiRenderComponent);
	}
}

void Core::GameObjectManager::FixedUpdate()
{
	for (std::unique_ptr<GameObject>& currentGO : m_gameObjects)
	{
		currentGO->FixedUpdate();
	}
}

#ifdef MODE_EDITOR
void Core::GameObjectManager::EditorUpdate()
{
	m_uiRenderComponent.clear();
	hdr->Update();

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->EditorUpdate();

		if (m_gameObjects[i]->HasDeleteFlag())
		{
			m_gameObjects.erase(m_gameObjects.begin() + i);
			i--;
			continue;
		}

		m_gameObjects[i]->RegisterUiComponent(m_uiRenderComponent);
	}
}

void Core::GameObjectManager::EditorFixedUpdate()
{
	for (std::unique_ptr<GameObject>& currentGO : m_gameObjects)
	{
		currentGO->EditorFixedUpdate();
	}
}

void Core::GameObjectManager::ShowManagerWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("GameObject Manager", p_opened);

	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		if (m_gameObjects[i]->GetId() == 0u)
			GameObject::SetupGameObjectID(m_gameObjects);

		if (ImGui::Selectable((m_gameObjects[i]->GetName() + "##" + std::to_string(m_gameObjects[i]->GetId())).c_str()))
			m_selectedGameObject = i;

		if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
		{
			int i_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
			if (i_next >= 0 && i_next < m_gameObjects.size())
			{
				m_selectedGameObject = i_next;
				m_gameObjects[i_next].swap(m_gameObjects[i]);
				ImGui::ResetMouseDragDelta();
			}
		}
	}


	static std::string newSceneName = "";
	ImGui::InputText("##GameObjectName", &newSceneName);
	ImGui::SameLine();
	if (ImGui::Button("Add GameObject##GameObjectName"))
		m_gameObjects.emplace_back(std::make_unique<GameObject>(newSceneName));

	ImGui::End();
}

void Core::GameObjectManager::ShowEditorWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("GameObject Editor", p_opened);

	if (m_selectedGameObject < m_gameObjects.size())
		m_gameObjects[m_selectedGameObject]->ShowEditorControl();
	else
		ImGui::Text("Aucun GameObject selectionne");

	ImGui::End();
}
#endif // MODE_EDITOR

void Core::GameObjectManager::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, hdr->m_hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (std::unique_ptr<GameObject>& currentGO : m_gameObjects)
	{
		currentGO->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hdr->Draw();
}

void Core::GameObjectManager::DrawUi()
{
	for (const std::pair<float, std::pair<GameObject*, ECS::UiRenderComponent*>>& currentuiComponent : m_uiRenderComponent)
		currentuiComponent.second.second->DrawUi(currentuiComponent.second.first);
}