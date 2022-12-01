#include "Core/GameObject.h"

#include "Core/Debug/Log.h"

#include "imgui.h"
#include "imgui_stdlib.h"

void Core::GameObject::Start()
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		currentComp->Start(this);
}

void Core::GameObject::Update()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->Enabled() && (m_components[i]->ComponentType() & ECS::COMPONENT_TYPE_GAMEPLAY))
		{
			ECS::GameplayComponent* AsGameplayComp = dynamic_cast<ECS::GameplayComponent*>(m_components[i].get());
			MYTH_ASSERT(AsGameplayComp != nullptr);
			AsGameplayComp->Update(this);
		}
		
		if (m_components[i]->HasDeleteFlag())
		{
			m_components.erase(m_components.begin() + i);
			i--;
			continue;
		}
	}
}

void Core::GameObject::FixedUpdate()
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		if (currentComp->Enabled() && (currentComp->ComponentType() & ECS::COMPONENT_TYPE_PHYSIC))
		{
			ECS::PhysicComponent* AsPhysicComp = dynamic_cast<ECS::PhysicComponent*>(currentComp.get());
			MYTH_ASSERT(AsPhysicComp != nullptr);
			AsPhysicComp->FixedUpdate(this);
		}
}
void Core::GameObject::setTransform(physx::PxTransform p_newTransform)
{
	ECS::Transform* transformComponent;
	if (TryGetComponent(&transformComponent) && !transformComponent->updatePhysicsTransform)
	{
		transformComponent->SetPosition({ p_newTransform.p.x,
											p_newTransform.p.y,
											p_newTransform.p.z });

		transformComponent->SetRotation({ p_newTransform.q.x,
											p_newTransform.q.y,
											p_newTransform.q.z,
											p_newTransform.q.w });
	}
}

#ifdef MODE_EDITOR
std::vector<const char*> Core::GameObject::m_componentName;
std::vector<std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)>> Core::GameObject::m_gComponentLoader;

void Core::GameObject::EditorUpdate()
{
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		if (m_components[i]->Enabled() && (m_components[i]->ComponentType() & ECS::COMPONENT_TYPE_GAMEPLAY))
		{
			ECS::GameplayComponent* AsGameplayComp = dynamic_cast<ECS::GameplayComponent*>(m_components[i].get());
			MYTH_ASSERT(AsGameplayComp != nullptr);
			AsGameplayComp->EditorUpdate(this);
		}

		if (m_components[i]->HasDeleteFlag())
		{
			m_components.erase(m_components.begin() + i);
			i--;
		}
	}
}

void Core::GameObject::EditorFixedUpdate()
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		if (currentComp->Enabled() && (currentComp->ComponentType() & ECS::COMPONENT_TYPE_PHYSIC))
		{
			ECS::PhysicComponent* AsPhysicComp = dynamic_cast<ECS::PhysicComponent*>(currentComp.get());
			MYTH_ASSERT(AsPhysicComp != nullptr);
			AsPhysicComp->EditorFixedUpdate(this);
		}
}

void Core::GameObject::ShowEditorControl()
{
	ImGui::InputText("##GameObjectName", &m_name);
	ImGui::SameLine();
	if (ImGui::Button("Delete##GameObject"))
		ImGui::OpenPopup("DeleteGameObject?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("DeleteGameObject?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("%s will be deleted.\nThis operation cannot be undone !\n\n", m_name.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{ 
			ImGui::CloseCurrentPopup();
			SetDeleteFlag(true);
		}
		
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	ImGui::Separator();

	bool IsChecked = false;
	for (unsigned int i = 0; i < m_components.size(); i++)
	{
		IsChecked = m_components[i]->Enabled();
		if (ImGui::Checkbox(("##EnabledComponent_" + std::to_string(i)).c_str(), &IsChecked))
			m_components[i]->SetEnabled(IsChecked);
		ImGui::SameLine();
		ImGui::Text("Component : %s", m_components[i]->GetTypeName());

		if (ImGui::Button(("Delete##Component_" + std::to_string(i)).c_str()))
			m_components[i]->SetDeleteFlag(true);

		m_components[i]->ShowEditorControl(i);
		ImGui::Separator();
	}

	unsigned int indexCombo = 0;
	if (ImGui::Combo("##comboBoxComponent", (int*)&indexCombo, m_componentName.data(), (int)m_componentName.size()))
	{
		if (indexCombo != 0)
		{
			indexCombo--;
			m_gComponentLoader[indexCombo](m_components);
		}
	}
}

void Core::GameObject::InitializeComponentType()
{
	m_componentName.insert(m_componentName.begin(), "Add component ...");

	MYTH_ASSERT(m_componentName.size() == (m_gComponentLoader.size() + (size_t)1));
}

void Core::GameObject::AddComponentType(const char* p_className, std::function<void(std::vector<std::unique_ptr<ECS::Component>>& lstComponent)> p_callback)
{
	m_componentName.push_back(p_className);
	m_gComponentLoader.push_back(p_callback);

	MYTH_ASSERT(m_componentName.size() == (m_gComponentLoader.size() + (size_t)1));
}

void Core::GameObject::SetupGameObjectID(std::vector<std::unique_ptr<GameObject>>& m_gameObjectsList)
{
	unsigned int newID = 1u;

	for (std::unique_ptr<GameObject>& currentGo : m_gameObjectsList)
	{
		currentGo->m_ID = newID;
		newID = newID + 1u;
	}
}
#endif

void Core::GameObject::Draw()
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		if (currentComp->Enabled() && (currentComp->ComponentType() & ECS::COMPONENT_TYPE_RENDER))
		{
			ECS::RenderComponent* AsRenderComp = dynamic_cast<ECS::RenderComponent*>(currentComp.get());
			MYTH_ASSERT(AsRenderComp != nullptr);
			AsRenderComp->Draw(this);
		}
}

/*void Core::GameObject::DrawUi()
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		if (currentComp->Enabled() && (currentComp->ComponentType() & ECS::COMPONENT_TYPE_UI_RENDER))
		{
			ECS::UiRenderComponent* AsRenderComp = dynamic_cast<ECS::UiRenderComponent*>(currentComp.get());
			MYTH_ASSERT(AsRenderComp != nullptr);
			AsRenderComp->DrawUi(this);
		}
}/**/

void Core::GameObject::RegisterUiComponent(std::map<float, std::pair<GameObject*, ECS::UiRenderComponent*>>& p_uiComponent)
{
	for (std::unique_ptr<ECS::Component>& currentComp : m_components)
		if (currentComp->Enabled() && (currentComp->ComponentType() & ECS::COMPONENT_TYPE_UI_RENDER))
		{
			ECS::UiRenderComponent* AsUirenderComp = dynamic_cast<ECS::UiRenderComponent*>(currentComp.get());
			MYTH_ASSERT(AsUirenderComp != nullptr);

			float layer = AsUirenderComp->GetLayer();
			while (p_uiComponent.find(layer) != p_uiComponent.end())
				layer = layer + 0.25f;

			p_uiComponent.insert({ layer, { this, AsUirenderComp } });
		}
}
