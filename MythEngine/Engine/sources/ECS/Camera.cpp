#include "ECS/Camera.h"

#include "Core/System/Application.h"
#include "ECS/Transform.h"
#include "Core/System/XMLMacro.h"

#include <miniaudio.h>

REGISTER_COMPONENT_CPP(ECS::Camera)

ECS::Camera::Camera(const bool p_isOrthographic) :
	m_isOrthographic(p_isOrthographic),
	m_isSelected(false),
	Position(0.f, 0.f, 5.f),
	Rotation(0.f, 0.f, 0.f, 1.f)
{
}

ECS::Camera::~Camera()
{
	if (Core::System::CameraManager::GetInstance().GetCamera() == this)
		Core::System::CameraManager::GetInstance().SetCamera(nullptr, false);
}

Mat4x4 ECS::Camera::GetViewMatrix() const
{
	//return Mat4x4::ViewRad(Pitch, Yaw, Position);
	return Rotation.GetRotationMatrix() * Mat4x4::Translate(-Position);
}

void ECS::Camera::Start(Core::GameObject* p_gameObject)
{
	Transform* TRS = nullptr;
	if (p_gameObject->TryGetComponent(&TRS, IndexTransform))
	{
		Position = TRS->GetGlobalPosition() + Offset;
		Rotation = TRS->GetGlobalRotation();

		if (m_isSelected)
		{
			ma_engine& currentEngine = Core::System::Application::GetInstance().GetAudioEngine();

			ma_engine_listener_set_position(&currentEngine, 0, Position.m_x, Position.m_y, Position.m_z);
			ma_engine_listener_set_direction(&currentEngine, 0,
				(2 * (Rotation.m_i * Rotation.m_k + Rotation.m_r * Rotation.m_j)),
				(2 * (Rotation.m_j * Rotation.m_k - Rotation.m_r * Rotation.m_i)),
				-(1 - 2 * (Rotation.m_i * Rotation.m_i + Rotation.m_j * Rotation.m_j))
			);
		}

		m_updateEventHandler = TRS->GetUpdateEvent().connect_member<ECS::Transform*>(*this, &ECS::Camera::TransformUpdate);
	}
}

void ECS::Camera::TransformUpdate(ECS::Transform* p_transform)
{
	Position = p_transform->GetGlobalPosition() + Offset;
	Rotation = p_transform->GetGlobalRotation();

	if (m_isSelected)
	{
		ma_engine& currentEngine = Core::System::Application::GetInstance().GetAudioEngine();

		ma_engine_listener_set_position(&currentEngine, 0, Position.m_x, Position.m_y, Position.m_z);
		ma_engine_listener_set_direction(&currentEngine, 0,
			(2 * (Rotation.m_i * Rotation.m_k + Rotation.m_r * Rotation.m_j)),
			(2 * (Rotation.m_j * Rotation.m_k - Rotation.m_r * Rotation.m_i)),
			-(1 - 2 * (Rotation.m_i * Rotation.m_i + Rotation.m_j * Rotation.m_j))
		);
	}
}

void ECS::Camera::Load(tinyxml2::XMLElement* ComponentXml)
{
	m_isOrthographic = ComponentXml->BoolAttribute("orthographic");

	if (ComponentXml->BoolAttribute("selected"))
		Core::System::CameraManager::GetInstance().SetCamera(this);

	IndexTransform = ComponentXml->UnsignedAttribute("index-transform");

	tinyxml2::XMLElement* OffsetXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &OffsetXML, "Offset"))
	{
		Offset.m_x = OffsetXML->FloatAttribute("X");
		Offset.m_y = OffsetXML->FloatAttribute("Y");
		Offset.m_z = OffsetXML->FloatAttribute("Z");
	}
}

#ifdef MODE_EDITOR

#include "imgui.h"

void ECS::Camera::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::SameLine();

	if (m_isSelected)
	{
		if (ImGui::Button(("Deselect##camera" + std::to_string(p_indexComponent)).c_str()))
			Core::System::CameraManager::GetInstance().SetCamera(nullptr);
	}
	else
	{
		if (ImGui::Button(("Select##camera" + std::to_string(p_indexComponent)).c_str()))
			Core::System::CameraManager::GetInstance().SetCamera(this);
	}

	ImGui::Spacing();

	ImGui::InputInt(("Transform##camera" + std::to_string(p_indexComponent)).c_str(), (int*)&IndexTransform);

	ImGui::Checkbox(("Is orthographic##camera" + std::to_string(p_indexComponent)).c_str(), &m_isOrthographic);
}

void ECS::Camera::Save(tinyxml2::XMLElement* ComponentXml)
{
	ComponentXml->SetAttribute("orthographic", m_isOrthographic);
	ComponentXml->SetAttribute("selected", m_isSelected);

	if (IndexTransform > 0u)
		ComponentXml->SetAttribute("index-transform", IndexTransform);

	tinyxml2::XMLElement* OffsetXML = ComponentXml->InsertNewChildElement("Offset");
	OffsetXML->SetAttribute("X", Offset.m_x);
	OffsetXML->SetAttribute("Y", Offset.m_y);
	OffsetXML->SetAttribute("Z", Offset.m_z);
}
#endif // MODE_EDITOR
