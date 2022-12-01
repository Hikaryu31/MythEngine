#include "Resources/ResourcesManager.h"

#include "imgui.h"
#include "imgui_stdlib.h"

bool Resources::ResourcesManager::RegisterAndLoadResource(std::shared_ptr<Resource> p_newResource)
{
	if (p_newResource == nullptr)
		return false;
	if (m_resources.find(p_newResource->GetName()) != m_resources.end())
		return false;

	p_newResource->LoadResource();
	m_resources.insert({ p_newResource->GetName(), p_newResource });

	return true;
}

bool Resources::ResourcesManager::RegisterAndLoadResource(Resource* p_newResource)
{
	if (p_newResource == nullptr)
		return false;
	if (m_resources.find(p_newResource->GetName()) != m_resources.end())
		return false;

	p_newResource->LoadResource();
	m_resources.insert({ p_newResource->GetName(), std::shared_ptr<Resource>(p_newResource) });

	return true;
}

void Resources::ResourcesManager::UnloadUnusedResource()
{
	for (const std::pair<std::string, std::shared_ptr<Resource>>& current : m_resources)
	{
		if (current.second.use_count() == 2l)
			current.second->UnloadResource();
	}

#ifdef MODE_EDITOR
	m_resourcesFilter.clear();
#endif // MODE_EDITOR
}

std::shared_ptr<Resources::Scene> Resources::ResourcesManager::GetScene(const std::string& p_sceneName)
{
	if (m_scenes.find(p_sceneName) == m_scenes.end())
		return nullptr;

	return m_scenes[p_sceneName];
}

bool Resources::ResourcesManager::AddScene(std::shared_ptr<Scene>& p_newScene)
{
	if (m_scenes.find(p_newScene->GetName()) != m_scenes.end())
		return false;

	p_newScene->SetResourcesManager(this);
	m_scenes.insert({ p_newScene->GetName(), p_newScene });

	return true;
}

#ifdef MODE_EDITOR
#include "Core/System/Application.h"

bool FindOrCreateFilter(std::vector<std::pair<std::string, bool>>& m_listFilter, const char* p_typeName)
{
	for (std::pair<std::string, bool>& tempFilter : m_listFilter)
	{
		if (tempFilter.first == p_typeName)
			return tempFilter.second;
	}

	m_listFilter.push_back({ p_typeName, true });
	return true;
}

void Resources::ResourcesManager::ShowManagerWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("Resources Manager", p_opened);

	if (ImGui::Button("Unload unused ressource"))
		UnloadUnusedResource();

	ImGui::SameLine();
	ImGui::Text("|");

	for (std::pair<std::string, bool>& filterInfoButton : m_resourcesFilter)
	{
		ImGui::SameLine();
		ImGui::Checkbox(filterInfoButton.first.c_str(), &filterInfoButton.second);
	}

	ImGui::Separator();

	ImGui::BeginChild("ListRessources");

	for (const std::pair<std::string, std::shared_ptr<Resource>>& currentResource : m_resources)
	{
		if (!FindOrCreateFilter(m_resourcesFilter, currentResource.second->GetTypeName()))
			continue;

		if (currentResource.second->IsLoaded())
			ImGui::Text(" - [ LOADED ]");
		else
			ImGui::TextColored({0.6f, 0.3f, 0.3f, 1.f}, " - [UNLOADED]");

		ImGui::SameLine();

		if (ImGui::Selectable(("[ " + std::string(currentResource.second->GetTypeName()) + " ] " + currentResource.first).c_str()))
			m_selected = currentResource.first;

		// drag sources here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			const char* currentName = currentResource.second->GetName().c_str();

			ImGui::SetDragDropPayload("DND_RESOURCE", currentName, sizeof(char) * (currentResource.second->GetName().length() + 1Ui64));

			ImGui::Text("%s", currentName);
			ImGui::EndDragDropSource();
		}
	}

	ImGui::EndChild();
	ImGui::End();
}

void Resources::ResourcesManager::ShowSceneWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("Scenes Manager", p_opened);

	static std::vector<std::string> tempSceneToDelete;

	std::string currentLoadedScene = "";
	if (Core::System::Application::GetInstance().GetGameObjectManager().HasScene())
		currentLoadedScene = Core::System::Application::GetInstance().GetGameObjectManager().GetCurrentScene()->GetName();

	for (const std::pair<std::string, std::shared_ptr<Scene>>& currentScene : m_scenes)
	{
		if (currentLoadedScene == currentScene.first)
		{
			ImGui::Text(" X %s", currentScene.first.c_str());

			ImGui::SameLine();
			if (ImGui::Button(("Reload##Scene" + currentScene.first).c_str()))
				Core::System::Application::QueryLoadScene(currentScene.first);

			if (currentScene.second->HasSaveSupport())
			{
				ImGui::SameLine();
				if (ImGui::Button(("Save##Scene" + currentScene.first).c_str()))
					currentScene.second->SaveGameObjects(/*m_resources,*/Core::System::Application::GetInstance().GetGameObjectManager().GetGameObjects());
			}
		}
		else
		{
			ImGui::Text(" - %s", currentScene.first.c_str());

			ImGui::SameLine();
			if (ImGui::Button(("Load##Scene" + currentScene.first).c_str()))
				Core::System::Application::QueryLoadScene(currentScene.first);

			ImGui::SameLine();
			if (ImGui::Button(("Delete##Scene" + currentScene.first).c_str()))
				ImGui::OpenPopup(("Delete Scene ?##Scene" + currentScene.first).c_str());

			if (currentScene.second->HasSaveSupport())
			{
				ImGui::SameLine();
				if (ImGui::Button(("Copy##Scene" + currentScene.first).c_str()))
					ImGui::OpenPopup(("Copy Scene ?##Scene" + currentScene.first).c_str());
			}
		}

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(("Copy Scene ?##Scene" + currentScene.first).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("this action will override \"%s\" content.\nThis operation cannot be undone !\n\n", currentScene.first.c_str());
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				currentScene.second->SaveGameObjects(/*m_resources,*/Core::System::Application::GetInstance().GetGameObjectManager().GetGameObjects());
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		// Always center this window when appearing
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(("Delete Scene ?##Scene" + currentScene.first).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("this action will delete \"%s\".\nThis operation cannot be undone !\n\n", currentScene.first.c_str());
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				tempSceneToDelete.push_back(currentScene.first);
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

	if (tempSceneToDelete.size() > 0Ui64)
	{
		for (const std::string& tempSceneToDelete : tempSceneToDelete)
			m_scenes.erase(tempSceneToDelete);

		tempSceneToDelete.clear();
	}

	static std::string newSceneName = "";
	ImGui::InputText("##SceneName", &newSceneName);
	ImGui::SameLine();
	if (ImGui::Button("Add Scene##SceneName"))
		if (m_scenes.find(newSceneName) == m_scenes.end())
			m_scenes.insert({ newSceneName, std::make_shared<BasicScene>(newSceneName) });

	ImGui::End();
}

void Resources::ResourcesManager::ShowEditorWindow(bool* p_opened)
{
	if (p_opened != nullptr)
		if (!(*p_opened))
			return;

	ImGui::Begin("Resources Editor", p_opened);

	static bool deleteCurrent = false;
	static std::string toDeleteCurrent = "";
	static bool directDelete = false;

	if (!m_selected.empty())
	{
		if (m_resources.find(m_selected) == m_resources.end())
			m_selected = "";
		else
		{
			ImGui::Text("Resource : %s\ntype : %s\nuse count = %ld", m_resources[m_selected]->GetName().c_str(), m_resources[m_selected]->GetTypeName(), m_resources[m_selected].use_count());

			ImGui::Spacing();

			if (ImGui::Button("Delete##Resource"))
			{
				deleteCurrent = true;
				toDeleteCurrent = m_selected;
				if (m_resources[m_selected].use_count() > 1)
					ImGui::OpenPopup("DeleteResource?");
				else
					directDelete = true;
			}
			ImGui::SameLine();
			if (m_resources[m_selected]->IsLoaded())
			{
				if (ImGui::Button("Unload##Resource"))
					m_resources[m_selected]->UnloadResource();
				ImGui::SameLine();
				if (ImGui::Button("Reload##Resource"))
				{
					m_resources[m_selected]->UnloadResource();
					m_resources[m_selected]->LoadResource();
				}
			}
			else
				if (ImGui::Button("Load##Resource"))
					m_resources[m_selected]->LoadResource();

			ImGui::Separator();
			m_resources[m_selected]->ShowEditorControl();
		}
	}
	else
		ImGui::Text("no Resource Selected");

	if (deleteCurrent)
	{
		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("DeleteResource?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s will be deleted, but he still used by component.\nload and unload while not be process by resources manager anymore !\n\n", m_resources[m_selected]->GetName().c_str());
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				directDelete = true;
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
				deleteCurrent = false;
			}

			ImGui::EndPopup();
		}

		if (directDelete)
		{
			m_resources.erase(m_selected);
			deleteCurrent = false;
			directDelete = false;
		}
	}

	ImGui::End();
}

#include "Core/System/XMLMacro.h"
void Resources::ResourcesManager::SaveScene(tinyxml2::XMLElement* SceneListXml)
{
	tinyxml2::XMLElement* currentXMLScene = nullptr;

	for (std::pair<const std::string, std::shared_ptr<Scene>>& currentSceneToSave : m_scenes)
		if (typeid(*(currentSceneToSave.second)) == typeid(Resources::BasicScene))
		{
			currentXMLScene = SceneListXml->InsertNewChildElement("scene");
			currentXMLScene->SetAttribute("name", currentSceneToSave.first.c_str());
		}
}
#endif // MODE_EDITOR
