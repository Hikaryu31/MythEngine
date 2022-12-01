#include "ECS/Transform.h"

#include "Core/System/XMLMacro.h"

#include "Core/System/EventManager.h"
#include "Core/GameObject.h"
#include "Core/System/Application.h"

#include "imgui.h"

REGISTER_COMPONENT_CPP(ECS::Transform)

ECS::Transform::Transform(Transform* p_parent) : 
	m_parent(p_parent),
	m_scale(Vec3::One),
	m_update(true)
{
	if (m_parent != nullptr)
		m_parent->AddChild(this);

	m_updateEvent = Core::System::EventManager::CreateNewEvent();
}

ECS::Transform::~Transform()
{
	if (m_parent != nullptr)
		m_parent->RemChild(this);

	for (Transform* child : m_childs)
		child->SetParent(nullptr, false);

	m_updateEvent = nullptr;
}

void ECS::Transform::SetParent(Transform* p_parent, bool unsetParent)
{
	if (p_parent == this)
	{
		MYTH_ERROR("impossible to make transform parent to himself !!");
		return;
	}

	if (m_parent != nullptr && unsetParent)
		m_parent->RemChild(this);

	m_parent = p_parent;

	if (m_parent != nullptr)
		m_parent->AddChild(this);
}

void ECS::Transform::AddChild(Transform* p_child)
{
	m_childs.push_back(p_child);
}

void ECS::Transform::RemChild(Transform* p_child)
{
	for (unsigned int i = 0; i < m_childs.size(); i++)
		if (m_childs[i] == p_child)
		{
			m_childs.erase(m_childs.begin() + i);
			i--;
		}
}

Vec3 ECS::Transform::GetGlobalPosition() const
{
	if (m_parent != nullptr)
	{
		return (Quaternion::Vec3RotateByQuat(m_position, m_parent->GetGlobalRotation()) * m_parent->GetGlobalScale()) + m_parent->GetGlobalPosition();
	}

	return m_position;
}

Quaternion ECS::Transform::GetGlobalRotation() const
{
	if (m_parent != nullptr)
	{
		return m_parent->GetGlobalRotation() * m_rotation;
	}

	return m_rotation;
}

Vec3 ECS::Transform::GetGlobalScale() const
{
	if (m_parent != nullptr)
	{
		return m_parent->GetGlobalScale() * m_scale;
	}

	return m_scale;
}

void ECS::Transform::Start(Core::GameObject* p_gameObject)
{
	m_update = true;
	UpdateTransform();
}

void ECS::Transform::FixedUpdate(Core::GameObject* p_gameObject)
{
	UpdateTransform();
}

void ECS::Transform::Load(tinyxml2::XMLElement* ComponentXml, Core::GameObject* p_gameObject)
{
	bool onSameGO = ComponentXml->BoolAttribute("same-GO", false);
	const char* ParentGameObject = ComponentXml->Attribute("parent");
	unsigned int indexTransform = ComponentXml->UnsignedAttribute("parent_index", 0u);
	
	if (ParentGameObject != nullptr)
		for (std::unique_ptr<Core::GameObject>& currentObj : Core::System::Application::GetInstance().GetGameObjectManager().GetGameObjects())
			if (currentObj->GetName() == ParentGameObject)
			{
				ECS::Transform* currentPrentTransform = nullptr;
				if (currentObj->TryGetComponent(&currentPrentTransform, indexTransform))
					SetParent(currentPrentTransform);
			}

	if (onSameGO)
	{
		ECS::Transform* currentPrentTransform = nullptr;
		if (p_gameObject->TryGetComponent(&currentPrentTransform, indexTransform))
			SetParent(currentPrentTransform);
	}

	tinyxml2::XMLElement* positionXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &positionXML, "position"))
	{
		m_position.m_x = positionXML->FloatAttribute("X");
		m_position.m_y = positionXML->FloatAttribute("Y");
		m_position.m_z = positionXML->FloatAttribute("Z");
	}

	tinyxml2::XMLElement* rotationXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &rotationXML, "rotation"))
	{
		m_rotation.m_i = rotationXML->FloatAttribute("I");
		m_rotation.m_j = rotationXML->FloatAttribute("J");
		m_rotation.m_k = rotationXML->FloatAttribute("K");
		m_rotation.m_r = rotationXML->FloatAttribute("R");
	}

	tinyxml2::XMLElement* scaleXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &scaleXML, "scale"))
	{
		m_scale.m_x = scaleXML->FloatAttribute("X", 1.f);
		m_scale.m_y = scaleXML->FloatAttribute("Y", 1.f);
		m_scale.m_z = scaleXML->FloatAttribute("Z", 1.f);
	}

	m_update = true;
}

#ifdef MODE_EDITOR
void ECS::Transform::UpdateTransform(bool updateEditorRotation)
#else
void ECS::Transform::UpdateTransform()
#endif // MODE_EDITOR
{
	if (!m_update)
		return;

	m_localMatrix = Mat4x4::Translate(m_position) *
		m_rotation.GetRotationMatrix() *
		Mat4x4::Scale(m_scale);

	if (m_parent != nullptr)
		m_globalMatrix = m_parent->GetGlobalMatrix() * m_localMatrix;
	else
		m_globalMatrix = m_localMatrix;

	for (Transform* curChildTransf : m_childs)
	{
		curChildTransf->m_update = true;
		curChildTransf->UpdateTransform();
	}

#ifdef MODE_EDITOR
	if (updateEditorRotation)
		m_editorRotation = m_rotation.ToEulerAngles();
#endif // MODE_EDITOR

	m_updateEvent->notify(this);

	m_update = false;

	if (m_updatePhysicsOnUpdate)
		updatePhysicsTransform = true;
	else
		m_updatePhysicsOnUpdate = true;
}

#ifdef MODE_EDITOR
void ECS::Transform::EditorFixedUpdate(Core::GameObject* p_gameObject)
{
	UpdateTransform(false);
}

std::vector<std::pair<std::string, ECS::Transform*>> ECS::Transform::listGlobalTransform;
int ECS::Transform::item_current_idx = 0;

void ECS::Transform::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGuizmo::SetID((int)p_indexComponent);

	static Mat4x4 mProj = Mat4x4::Identity;
	static Mat4x4 mView = Mat4x4::Identity;

	if (m_parent != nullptr)
	{
		ImGui::Text("Parent : %p", m_parent);
		ImGui::SameLine();
		if (ImGui::Button(("Remove parent##transform-" + std::to_string(p_indexComponent)).c_str()))
		{
			Mat4x4 TranfByParent = m_globalMatrix;

			SetParent(nullptr);

			ImGuizmo::DecomposeMatrixToComponents((float*)&TranfByParent, (float*)&m_position, (float*)&m_editorRotation, (float*)&m_scale);

			m_editorRotation.m_x = Maths::Mod((m_editorRotation.m_x + TAU) + PI, TAU) - PI; // Loop around -180,180
			m_editorRotation.m_y = Maths::Mod((m_editorRotation.m_y + TAU) + PI, TAU) - PI; // Loop around -180,180
			m_editorRotation.m_z = Maths::Mod((m_editorRotation.m_z + TAU) + PI, TAU) - PI; // Loop around -180,180

			m_rotation = Quaternion::GetQuatFromEuler(m_editorRotation);

			m_update = true;
		}
	}
	else
	{
		ImGui::Text("Parent : NONE");
		ImGui::SameLine();
		if (ImGui::Button(("Select parent##transform-" + std::to_string(p_indexComponent)).c_str()))
		{
			listGlobalTransform.clear();

			for (std::unique_ptr<Core::GameObject>& currentObj : Core::System::Application::GetInstance().GetGameObjectManager().GetGameObjects())
			{
				std::vector<Transform*> curListTransf = currentObj->GetAllComponents<Transform>();

				for (Transform* curTransfo : curListTransf)
					listGlobalTransform.push_back({ currentObj->GetName(), curTransfo });
			}

			ImGui::OpenPopup(("Select Tranform Parent##For" + std::to_string(p_indexComponent)).c_str());
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(("Select Tranform Parent##For" + std::to_string(p_indexComponent)).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::BeginListBox("##list transform", ImVec2(400, 0)))
			{
				for (unsigned int n = 0; n < listGlobalTransform.size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable((listGlobalTransform[n].first + " - " + std::to_string(n)).c_str(), is_selected))
						item_current_idx = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			ImGui::Separator();

			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();

				Mat4x4 TranfByParent = listGlobalTransform[item_current_idx].second->GetGlobalMatrix().GetInverse() * m_localMatrix;

				ImGuizmo::DecomposeMatrixToComponents((float*)&TranfByParent, (float*)&m_position, (float*)&m_editorRotation, (float*)&m_scale);

				m_editorRotation.m_x = Maths::Mod((m_editorRotation.m_x + TAU) + PI, TAU) - PI; // Loop around -180,180
				m_editorRotation.m_y = Maths::Mod((m_editorRotation.m_y + TAU) + PI, TAU) - PI; // Loop around -180,180
				m_editorRotation.m_z = Maths::Mod((m_editorRotation.m_z + TAU) + PI, TAU) - PI; // Loop around -180,180

				m_rotation = Quaternion::GetQuatFromEuler(m_editorRotation);

				m_update = true;

				SetParent(listGlobalTransform[item_current_idx].second);
			}

			ImGui::SameLine();
			if (ImGui::Button("OK (keep value)", ImVec2(140, 0)))
			{
				ImGui::CloseCurrentPopup();
				m_update = true;
				SetParent(listGlobalTransform[item_current_idx].second);
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

	ImGui::Spacing();

	if (ImGui::RadioButton(("Translate##Gizmotransform-" + std::to_string(p_indexComponent)).c_str(), mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton(("Rotate##Gizmotransform-" + std::to_string(p_indexComponent)).c_str(), mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton(("Scale##Gizmotransform-" + std::to_string(p_indexComponent)).c_str(), mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::DragFloat3(("position##transform-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_position, 0.1f))
	{
		m_update = true;
	}

	static bool showAsQuaternion = false;
	if (showAsQuaternion)
	{
		if (ImGui::DragFloat4(("rotation##Qtransform-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_rotation, 0.01f))
		{
			m_update = true;
		}
	}
	else
	{
		if (ImGui::DragFloat3(("rotation##EAtransform-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_editorRotation, 0.01f))
		{
			m_editorRotation.m_x = Maths::Mod((m_editorRotation.m_x + TAU) + PI, TAU) - PI; // Loop around -180,180
			m_editorRotation.m_y = Maths::Mod((m_editorRotation.m_y + TAU) + PI, TAU) - PI; // Loop around -180,180
			m_editorRotation.m_z = Maths::Mod((m_editorRotation.m_z + TAU) + PI, TAU) - PI; // Loop around -180,180

			m_rotation = Quaternion::GetQuatFromEuler(m_editorRotation);

			m_update = true;
		}
	}

	ImGui::SameLine();
	ImGui::Checkbox(("##rotationTypeTransform-" + std::to_string(p_indexComponent)).c_str(), &showAsQuaternion);

	static bool fullScaleModification = false;
	if (fullScaleModification)
	{
		if (ImGui::DragFloat3(("scale##3transform-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_scale, 0.1f))
			m_update = true;
	}
	else
	{
		if (ImGui::DragFloat(("scale##1transform-" + std::to_string(p_indexComponent)).c_str(), (float*)&(m_scale.m_x), 0.1f))
		{
			m_scale.m_y = m_scale.m_z = m_scale.m_x;

			m_update = true;
		}
	}

	ImGui::SameLine();
	ImGui::Checkbox(("##scaleTypeTransform-" + std::to_string(p_indexComponent)).c_str(), &fullScaleModification);

	Core::System::CameraManager& CM = Core::System::CameraManager::GetInstance();

	mProj = CM.GetProjectionMatrix();
	mView = Mat4x4::ViewRad(0.f, 0.f, { 0.f, 0.f, 5.f });
	if (CM.HasCamera())
		mView = CM.GetCamera()->GetViewMatrix();

	//ImGuizmo::RecomposeMatrixFromComponents((float*)&m_position, (float*)&m_editorRotation, (float*)&m_scale, (float*)&mCurrentGuizmoMatrix);
	mCurrentGuizmoMatrix = GetGlobalMatrix();

	ImGuizmo::SetRect(0, 0, (float)CM.WindowWidth, (float)CM.WindowHeight);

	if (ImGuizmo::Manipulate((float*)&mView, (float*)&mProj, mCurrentGizmoOperation, ImGuizmo::MODE::WORLD, (float*)&mCurrentGuizmoMatrix))
	{
		//this->SetMatrixFromWorld(mCurrentGuizmoMatrix);
		if (m_parent != nullptr)
		{
			Mat4x4 invp = m_parent->GetGlobalMatrix().GetInverse();
			mCurrentGuizmoMatrix = invp * mCurrentGuizmoMatrix;
		}

		ImGuizmo::DecomposeMatrixToComponents((float*)&mCurrentGuizmoMatrix, (float*)&m_position, (float*)&m_editorRotation, (float*)&m_scale);

		m_editorRotation.m_x = Maths::Mod((m_editorRotation.m_x + TAU) + PI, TAU) - PI; // Loop around -180,180
		m_editorRotation.m_y = Maths::Mod((m_editorRotation.m_y + TAU) + PI, TAU) - PI; // Loop around -180,180
		m_editorRotation.m_z = Maths::Mod((m_editorRotation.m_z + TAU) + PI, TAU) - PI; // Loop around -180,180

		m_rotation = Quaternion::GetQuatFromEuler(m_editorRotation);

		m_update = true;
	}
}

void ECS::Transform::Save(tinyxml2::XMLElement* ComponentXml)
{
	tinyxml2::XMLElement* positionXML = ComponentXml->InsertNewChildElement("position");
	positionXML->SetAttribute("X", m_position.m_x);
	positionXML->SetAttribute("Y", m_position.m_y);
	positionXML->SetAttribute("Z", m_position.m_z);

	tinyxml2::XMLElement* rotationXML = ComponentXml->InsertNewChildElement("rotation");
	rotationXML->SetAttribute("I", m_rotation.m_i);
	rotationXML->SetAttribute("J", m_rotation.m_j);
	rotationXML->SetAttribute("K", m_rotation.m_k);
	rotationXML->SetAttribute("R", m_rotation.m_r);

	tinyxml2::XMLElement* scaleXML = ComponentXml->InsertNewChildElement("scale");
	scaleXML->SetAttribute("X", m_scale.m_x);
	scaleXML->SetAttribute("Y", m_scale.m_y);
	scaleXML->SetAttribute("Z", m_scale.m_z);

	if (m_parent != nullptr)
	{
		std::string gameObjectParentName = "";
		unsigned int transformIndex = 0u;
		bool hasSameGo = false;

		for (std::unique_ptr<Core::GameObject>& currentObj : Core::System::Application::GetInstance().GetGameObjectManager().GetGameObjects())
		{
			std::vector<Transform*> curListTransf = currentObj->GetAllComponents<Transform>();
			bool tempHasSameGo = false;
			bool findParentInThisObject = false;

			for (unsigned int i = 0u; i < curListTransf.size(); i++)
			{
				if (curListTransf[i] == this)
				{
					tempHasSameGo = true;

					if (findParentInThisObject)
						hasSameGo = true;

					continue;
				}

				if (curListTransf[i] == m_parent)
				{
					gameObjectParentName = currentObj->GetName();
					transformIndex = i;
					hasSameGo = tempHasSameGo;
					findParentInThisObject = true;
				}
			}
		}

		if (hasSameGo)
			ComponentXml->SetAttribute("same-GO", true);
		else
			ComponentXml->SetAttribute("parent", gameObjectParentName.c_str());

		ComponentXml->SetAttribute("parent_index", transformIndex);
	}
}
#endif // MODE_EDITOR