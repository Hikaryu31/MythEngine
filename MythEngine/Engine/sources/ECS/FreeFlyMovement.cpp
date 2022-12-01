#include "ECS/FreeFlyMovement.h"

#include "ECS/Transform.h"
#include "Core/GameObject.h"

#include "Core/System/Input.h"
#include "Core/System/Time.h"

REGISTER_COMPONENT_CPP(ECS::FreeFlyMovement)

ECS::FreeFlyMovement::FreeFlyMovement(const unsigned int p_TrIndex, const float p_speed, const float p_maxSpeed) :
	m_indexTransform(p_TrIndex),
	m_speed(p_speed),
	m_maxSpeed(p_maxSpeed)
{
}

ECS::FreeFlyMovement::~FreeFlyMovement()
{
}

void ECS::FreeFlyMovement::Update(Core::GameObject* p_gameObject)
{
	Core::System::InputStatus LookInputStatus = Core::System::Input::GetKeyInputStatus(Core::System::Input::GetKeyInputIndex(CAMERA_LOOK_INPUT_NAME));
	if (LookInputStatus.IsPressed) Core::System::Input::GetInstance().SetMouseLock(true);
	if (LookInputStatus.IsReleased) Core::System::Input::GetInstance().SetMouseLock(false);
	if (!LookInputStatus.IsDown) return;

	Transform* transform = nullptr;
	if (!p_gameObject->TryGetComponent<Transform>(&transform, m_indexTransform))
		return;

	Mat4x4 mat = transform->GetGlobalMatrix().GetInverse();
	//Quaternion Rotat = transform->GetRotation();
	Vec3 ForwardVec = -mat.GetColumn(2).Get3D();
	//ForwardVec.m_z = -ForwardVec.m_z;
	Vec3 RightVec = mat.GetColumn(0).Get3D();
	//RightVec.m_z = -RightVec.m_z;

	Vec3& tPosition = transform->Position();

	float FrameSpeed = (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_SPEED_INPUT_NAME)) ? m_maxSpeed : m_speed) * Core::System::Time::DeltaTime();

	float ForwardVelocity = 0.f;
	float StrafeVelocity = 0.f;

	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_FRONT_INPUT_NAME))) ForwardVelocity += +FrameSpeed;
	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_BACK_INPUT_NAME))) ForwardVelocity += -FrameSpeed;

	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_LEFT_INPUT_NAME))) StrafeVelocity += -FrameSpeed;
	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_RIGHT_INPUT_NAME))) StrafeVelocity += +FrameSpeed;

	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_UP_INPUT_NAME))) tPosition.m_y += FrameSpeed;
	if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_DOWN_INPUT_NAME))) tPosition.m_y -= FrameSpeed;

	// Forward movement
	tPosition += ForwardVec * ForwardVelocity;

	// Strafe movement
	tPosition += RightVec * StrafeVelocity;

	
	Vec2 DeltaMouse = Core::System::Input::GetMouseDelta();

	//if (Core::System::Input::IsKeyDown(Core::System::Input::GetKeyInputIndex(CAMERA_ROLL_INPUT_NAME)))
	//	transform->Rotation() *= Quaternion(Rotat.Foreward(), DeltaMouse.m_x * CAM_MOUSE_SENSITIVITY_X);
	//else
	//{
		transform->Rotation() *= Quaternion(Vec3(0.f, 1.f, 0.f), DeltaMouse.m_x * CAM_MOUSE_SENSITIVITY_X);

		Vec3 RightVector = transform->GetRotation().Right();
		RightVector.m_y = 0.f;
		RightVector.Normalize();
		transform->Rotation() *= Quaternion(RightVector, DeltaMouse.m_y * CAM_MOUSE_SENSITIVITY_Y);
		//transform->Rotation() *= Quaternion(transform->GetRotation().Right(), DeltaMouse.m_y * CAM_MOUSE_SENSITIVITY_Y);
	//}
}

void ECS::FreeFlyMovement::Load(tinyxml2::XMLElement* ComponentXml)
{
	m_speed = ComponentXml->FloatAttribute("speed", 4.f);
	m_maxSpeed = ComponentXml->FloatAttribute("max-speed", 40.f);

	m_indexTransform = ComponentXml->UnsignedAttribute("index-transform");
}

#ifdef MODE_EDITOR
void ECS::FreeFlyMovement::EditorUpdate(Core::GameObject* p_gameObject)
{
	Update(p_gameObject);
}

#include "imgui.h"

void ECS::FreeFlyMovement::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::Spacing();

	ImGui::InputInt(("Transform##FreeFlyMovement" + std::to_string(p_indexComponent)).c_str(), (int*)&m_indexTransform);
	ImGui::DragFloat(("Speed##FreeFlyMovement" + std::to_string(p_indexComponent)).c_str(), &m_speed, 0.1f);
	ImGui::DragFloat(("Max speed##FreeFlyMovement" + std::to_string(p_indexComponent)).c_str(), &m_maxSpeed, 0.1f);
}

void ECS::FreeFlyMovement::Save(tinyxml2::XMLElement* ComponentXml)
{
	ComponentXml->SetAttribute("speed", m_speed);
	ComponentXml->SetAttribute("max-speed", m_maxSpeed);

	if (m_indexTransform > 0u)
		ComponentXml->SetAttribute("index-transform", m_indexTransform);
}
#endif // MODE_EDITOR
