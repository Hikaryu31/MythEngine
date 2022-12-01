#include "ECS/Light.h"
#include "Core/GameObject.h"
#include "Core/System/LightManager.h"

REGISTER_COMPONENT_CPP(ECS::PointLight)

ECS::PointLight::PointLight() : Component()
{
	Core::System::LightManager::RegisterPointLight(this);
}

ECS::PointLight::~PointLight()
{
	Core::System::LightManager::UnregisterPointLight(this);
}

void ECS::PointLight::Start(Core::GameObject* p_gameObject)
{
	Transform* TRS = nullptr;
	if (p_gameObject->TryGetComponent(&TRS))
		Position = TRS->GetGlobalPosition();
}

void ECS::PointLight::Update(Core::GameObject* p_gameObject)
{
	Transform* TRS = nullptr;
	if (p_gameObject->TryGetComponent(&TRS))
		Position = TRS->GetGlobalPosition();
}

#include "Core/System/XMLMacro.h"
void ECS::PointLight::Load(tinyxml2::XMLElement* ComponentXml)
{
	tinyxml2::XMLElement* dataXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &dataXML, "data"))
	{
		Constant = dataXML->FloatAttribute("constant", 0.f);
		Linear = dataXML->FloatAttribute("linear", 0.f);
		Quadratic = dataXML->FloatAttribute("quadratic", 0.f);
	}

	tinyxml2::XMLElement* ambientXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &ambientXML, "ambient"))
	{
		Ambient.m_x = ambientXML->FloatAttribute("R", 0.1f);
		Ambient.m_y = ambientXML->FloatAttribute("G", 0.1f);
		Ambient.m_z = ambientXML->FloatAttribute("B", 0.1f);
	}

	tinyxml2::XMLElement* diffuseXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &diffuseXML, "diffuse"))
	{
		Diffuse.m_x = diffuseXML->FloatAttribute("R", 1.f);
		Diffuse.m_y = diffuseXML->FloatAttribute("G", 1.f);
		Diffuse.m_z = diffuseXML->FloatAttribute("B", 1.f);
	}

	tinyxml2::XMLElement* specularXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &specularXML, "specular"))
	{
		Specular.m_x = specularXML->FloatAttribute("R", 1.f);
		Specular.m_y = specularXML->FloatAttribute("G", 1.f);
		Specular.m_z = specularXML->FloatAttribute("B", 1.f);
	}
}

#ifdef MODE_EDITOR
#include "imgui.h"
#include <fmt/format.h>
void ECS::PointLight::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::Spacing();

	ImGui::Text(fmt::format("Position x : {}, y : {}, z : {}", Position.m_x, Position.m_y, Position.m_z).c_str());

	ImGui::Spacing();

	ImGui::DragFloat(("Constant##PointLight-" + std::to_string(p_indexComponent)).c_str(), &Constant, 0.001f, 0.f, 1.f);
	ImGui::DragFloat(("Linear##PointLight-" + std::to_string(p_indexComponent)).c_str(), &Linear, 0.001f, 0.f, 1.f);
	ImGui::DragFloat(("Quadratic##PointLight-" + std::to_string(p_indexComponent)).c_str(), &Quadratic, 0.001f, 0.f, 1.f);

	ImGui::Spacing();

	ImGui::ColorEdit3(("Ambient##PointLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Ambient), ImGuiColorEditFlags_HDR);
	ImGui::ColorEdit3(("Diffuse##PointLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Diffuse), ImGuiColorEditFlags_HDR);
	ImGui::ColorEdit3(("Specular##PointLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Specular), ImGuiColorEditFlags_HDR);
}

void ECS::PointLight::Save(tinyxml2::XMLElement* ComponentXml)
{
	tinyxml2::XMLElement* dataXML = ComponentXml->InsertNewChildElement("data");
	dataXML->SetAttribute("constant", Constant);
	dataXML->SetAttribute("linear", Linear);
	dataXML->SetAttribute("quadratic", Quadratic);

	tinyxml2::XMLElement* ambientXML = ComponentXml->InsertNewChildElement("ambient");
	ambientXML->SetAttribute("R", Ambient.m_x);
	ambientXML->SetAttribute("G", Ambient.m_y);
	ambientXML->SetAttribute("B", Ambient.m_z);

	tinyxml2::XMLElement* diffuseXML = ComponentXml->InsertNewChildElement("diffuse");
	diffuseXML->SetAttribute("R", Diffuse.m_x);
	diffuseXML->SetAttribute("G", Diffuse.m_y);
	diffuseXML->SetAttribute("B", Diffuse.m_z);

	tinyxml2::XMLElement* specularXML = ComponentXml->InsertNewChildElement("specular");
	specularXML->SetAttribute("R", Specular.m_x);
	specularXML->SetAttribute("G", Specular.m_y);
	specularXML->SetAttribute("B", Specular.m_z);
}
#endif // MODE_EDITOR

// =========================== DirectionLight ===========================

REGISTER_COMPONENT_CPP(ECS::DirectionLight)

ECS::DirectionLight::~DirectionLight()
{
	if (Core::System::LightManager::GetDirectionLight() == this)
		Core::System::LightManager::SelectPointLight(nullptr, false);
}

void ECS::DirectionLight::Start(Core::GameObject* p_gameObject)
{
	Transform* transform = nullptr;
	if (!p_gameObject->TryGetComponent<Transform>(&transform))
		return;

	Direction = -transform->GetGlobalPosition();
	Direction.Normalize();
}

void ECS::DirectionLight::Update(Core::GameObject* p_gameObject)
{
	Transform* transform = nullptr;
	if (!p_gameObject->TryGetComponent<Transform>(&transform))
		return;

	Direction = -transform->GetGlobalPosition();
	Direction.Normalize();
}

void ECS::DirectionLight::Load(tinyxml2::XMLElement* ComponentXml)
{
	if (ComponentXml->BoolAttribute("selected"))
		Core::System::LightManager::SelectPointLight(this);

	tinyxml2::XMLElement* ambientXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &ambientXML, "ambient"))
	{
		Ambient.m_x = ambientXML->FloatAttribute("R", 0.1f);
		Ambient.m_y = ambientXML->FloatAttribute("G", 0.1f);
		Ambient.m_z = ambientXML->FloatAttribute("B", 0.1f);
	}

	tinyxml2::XMLElement* diffuseXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &diffuseXML, "diffuse"))
	{
		Diffuse.m_x = diffuseXML->FloatAttribute("R", 1.f);
		Diffuse.m_y = diffuseXML->FloatAttribute("G", 1.f);
		Diffuse.m_z = diffuseXML->FloatAttribute("B", 1.f);
	}

	tinyxml2::XMLElement* specularXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &specularXML, "specular"))
	{
		Specular.m_x = specularXML->FloatAttribute("R", 1.f);
		Specular.m_y = specularXML->FloatAttribute("G", 1.f);
		Specular.m_z = specularXML->FloatAttribute("B", 1.f);
	}
}

#ifdef MODE_EDITOR
void ECS::DirectionLight::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::SameLine();

	if (m_selected)
	{
		if (ImGui::Button(("Deselect##camera" + std::to_string(p_indexComponent)).c_str()))
			Core::System::LightManager::SelectPointLight(nullptr);
	}
	else
	{
		if (ImGui::Button(("Select##camera" + std::to_string(p_indexComponent)).c_str()))
			Core::System::LightManager::SelectPointLight(this);
	}

	ImGui::Spacing();

	ImGui::ColorEdit3(("Ambient##DirectionLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Ambient));
	ImGui::ColorEdit3(("Diffuse##DirectionLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Diffuse));
	ImGui::ColorEdit3(("Specular##DirectionLight-" + std::to_string(p_indexComponent)).c_str(), (float*)(&Specular));
}

void ECS::DirectionLight::Save(tinyxml2::XMLElement* ComponentXml)
{
	ComponentXml->SetAttribute("selected", m_selected);

	tinyxml2::XMLElement* ambientXML = ComponentXml->InsertNewChildElement("ambient");
	ambientXML->SetAttribute("R", Ambient.m_x);
	ambientXML->SetAttribute("G", Ambient.m_y);
	ambientXML->SetAttribute("B", Ambient.m_z);

	tinyxml2::XMLElement* diffuseXML = ComponentXml->InsertNewChildElement("diffuse");
	diffuseXML->SetAttribute("R", Diffuse.m_x);
	diffuseXML->SetAttribute("G", Diffuse.m_y);
	diffuseXML->SetAttribute("B", Diffuse.m_z);

	tinyxml2::XMLElement* specularXML = ComponentXml->InsertNewChildElement("specular");
	specularXML->SetAttribute("R", Specular.m_x);
	specularXML->SetAttribute("G", Specular.m_y);
	specularXML->SetAttribute("B", Specular.m_z);
}
#endif // MODE_EDITOR