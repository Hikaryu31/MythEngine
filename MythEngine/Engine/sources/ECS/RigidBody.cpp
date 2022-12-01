#include "ECS/RigidBody.h"
#include "Core/System/XMLMacro.h"
#include <float.h>
REGISTER_COMPONENT_CPP(ECS::RigidBody)

using namespace physx;

ECS::RigidBody::RigidBody()
{
	Core::System::Application& appInstance = Core::System::Application::GetInstance();

	m_actor = appInstance.m_physicSystem->m_physics->createRigidDynamic(PxTransform(PxVec3(0)));
	m_material= appInstance.m_physicSystem->m_physics->createMaterial(0.5f, 0.5f, 0.1f);
	PxRigidActorExt::createExclusiveShape(*m_actor, 
										  PxBoxGeometry(m_boxScale.m_x, m_boxScale.m_y, m_boxScale.m_z), 
										  *m_material);
	appInstance.m_physicSystem->m_scene->addActor(*m_actor);
}

ECS::RigidBody::~RigidBody()
{
	if (m_actor)
	{
		Core::System::Application::GetInstance().m_physicSystem->RemoveActor(m_actor);
	}
}

/////////////////////////////////////////////////////  Public methods  /////////////////////////////////////////////////////

void ECS::RigidBody::Load(tinyxml2::XMLElement* ComponentXml)
{
	tinyxml2::XMLElement* boxScaleXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &boxScaleXML, "boxScale"))
	{
		m_boxScale.m_x = boxScaleXML->FloatAttribute("X");
		m_boxScale.m_y = boxScaleXML->FloatAttribute("Y");
		m_boxScale.m_z = boxScaleXML->FloatAttribute("Z");
	}
	
	tinyxml2::XMLElement* capsuleScaleXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &capsuleScaleXML, "capsuleScale"))
	{
		m_capsuleHalfHeight = capsuleScaleXML->FloatAttribute("halfHeight");
		m_capsuleRadius = capsuleScaleXML->FloatAttribute("capsuleRadius");
	}

	tinyxml2::XMLElement* sphereScaleXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &sphereScaleXML, "sphereScale"))
		m_sphereRadius = sphereScaleXML->FloatAttribute("sphereRadius");

	tinyxml2::XMLElement* booleansXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &booleansXML, "booleans"))
	{
#ifdef MODE_EDITOR
		m_drawCollider = booleansXML->BoolAttribute("drawCollider");
#endif // MODE_EDITOR
		m_isTrigger = booleansXML->BoolAttribute("isTrigger");
		m_isKinematic = booleansXML->BoolAttribute("isKinematic");
		m_useGravity = booleansXML->BoolAttribute("useGravity");
		SetUsingGravity();
	}

	tinyxml2::XMLElement* shapeXML = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &shapeXML, "shape"))
		m_shape = shapeXML->IntAttribute("type");

	tinyxml2::XMLElement* offset = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &offset, "offset"))
	{
		m_colliderOffset.m_x = offset->FloatAttribute("offsetX");
		m_colliderOffset.m_y = offset->FloatAttribute("offsetY");
		m_colliderOffset.m_z = offset->FloatAttribute("offsetZ");
	}

	tinyxml2::XMLElement* constraints = nullptr;
	if (XMLMacro::GetElement(ComponentXml, &constraints, "constraints"))
	{
		m_lockTranslationAxisX = constraints->BoolAttribute("translationX");
		m_lockTranslationAxisY = constraints->BoolAttribute("translationY");
		m_lockTranslationAxisZ = constraints->BoolAttribute("translationZ");

		m_lockRotationAxisX = constraints->BoolAttribute("rotationX");
		m_lockRotationAxisY = constraints->BoolAttribute("rotationY");
		m_lockRotationAxisZ = constraints->BoolAttribute("rotationZ");
	}
	SetCollider();
	SetConstraints();
}


void ECS::RigidBody::Start(Core::GameObject* p_gameObject)
{
	ECS::Transform* currentTran = nullptr;
	if (p_gameObject->TryGetComponent(&currentTran))
	{
		Vec3 p = currentTran->GetGlobalPosition();
		Quaternion q = currentTran->GetGlobalRotation();
		m_actor->setGlobalPose(PxTransform(PxVec3(p.m_x, p.m_y, p.m_z), PxQuat(q.m_i, q.m_j, q.m_k, q.m_r)), false);
	}

	m_newOffset = true;
	UpdateOffset();
	SetTrigger();
	SetKinematic();
	SetCollider();
	m_actor->userData = p_gameObject;
}

void ECS::RigidBody::Update(Core::GameObject* p_gameObject)
{
	UpdateColliderPos(p_gameObject);
}

void ECS::RigidBody::UpdateColliderPos(Core::GameObject* p_gameObject)
{
	ECS::Transform* trComp;

	if (p_gameObject->TryGetComponent(&trComp) && trComp->updatePhysicsTransform)
	{
		trComp->updatePhysicsTransform = false;
		Vec3 p = trComp->GetGlobalPosition();
		Quaternion q = trComp->GetGlobalRotation();
		m_actor->setGlobalPose(PxTransform(PxVec3(p.m_x, p.m_y, p.m_z), PxQuat(q.m_i, q.m_j, q.m_k, q.m_r)));
	}
}

void ECS::RigidBody::UpdateOffset()
{
	if (m_newOffset)
	{
		m_newOffset = false;
		PxShape* shape = nullptr;
		m_actor->getShapes(&shape, 1);
		if (shape)
			shape->setLocalPose(PxTransform(PxVec3(m_colliderOffset.m_x, m_colliderOffset.m_y, m_colliderOffset.m_z)));
	}
}

#ifdef MODE_EDITOR

//From this line to line 350, ask Gabriel for details

static const float g_vertex_buffer_data[] = {
	-1.f,-1.f,-1.f,
	-1.f,-1.f, 1.f,
	-1.f, 1.f, 1.f,
	 1.f, 1.f,-1.f,
	-1.f,-1.f,-1.f,
	-1.f, 1.f,-1.f,

	 1.f,-1.f, 1.f,
	-1.f,-1.f,-1.f,
	 1.f,-1.f,-1.f,
	 1.f, 1.f,-1.f,
	 1.f,-1.f,-1.f,
	-1.f,-1.f,-1.f,

	-1.f,-1.f,-1.f,
	-1.f, 1.f, 1.f,
	-1.f, 1.f,-1.f,
	 1.f,-1.f, 1.f,
	-1.f,-1.f, 1.f,
	-1.f,-1.f,-1.f,

	-1.f, 1.f, 1.f,
	-1.f,-1.f, 1.f,
	 1.f,-1.f, 1.f,
	 1.f, 1.f, 1.f,
	 1.f,-1.f,-1.f,
	 1.f, 1.f,-1.f,

	 1.f,-1.f,-1.f,
	 1.f, 1.f, 1.f,
	 1.f,-1.f, 1.f,
	 1.f, 1.f, 1.f,
	 1.f, 1.f,-1.f,
	-1.f, 1.f,-1.f,

	 1.f, 1.f, 1.f,
	-1.f, 1.f,-1.f,
	-1.f, 1.f, 1.f,
	 1.f, 1.f, 1.f,
	-1.f, 1.f, 1.f,
	 1.f,-1.f, 1.f
};
static GLuint vertexBuffer = 0u;
static GLuint vertexArray = 0u;
static GLuint shaderCollider = 0u;

void ECS::RigidBody::Draw(Core::GameObject* currentGo)
{
	if (!m_drawCollider)
		return;

	if (vertexBuffer == 0u)
	{
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	}

	if (shaderCollider == 0u)
	{
		GLint Result = GL_FALSE;
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		//int InfoLogLength = 1024;

		// Compile Vertex Shader
		char const* VertexSourcePointer = R"GLSL(
#version 450 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uVP;
uniform mat4 uModel;

void main()
{
	gl_Position = uVP * uModel * vec4(aPosition, 1.0);
}
		)GLSL";

		glShaderSource(vertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(vertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(vertexShaderID, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("VShader error: %s\n", Infolog);
		}

		// Compile Fragment Shader
		char const* FragmentSourcePointer = R"GLSL(
#version 450 core

uniform vec4 uColor;
out vec4 oColor;

void main()
{
	oColor = uColor;
}
		)GLSL";
		glShaderSource(fragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(fragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(fragmentShaderID, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("FShader error: %s\n", Infolog);
		}

		// Link the program
		shaderCollider = glCreateProgram();
		glAttachShader(shaderCollider, vertexShaderID);
		glAttachShader(shaderCollider, fragmentShaderID);
		glLinkProgram(shaderCollider);

		// Check Fragment Shader
		glGetShaderiv(shaderCollider, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(shaderCollider, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(shaderCollider, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("Program Shader error: %s\n", Infolog);
		}

		glDetachShader(shaderCollider, vertexShaderID);
		glDetachShader(shaderCollider, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(shaderCollider);

	glProgramUniform4f(shaderCollider, glGetUniformLocation(shaderCollider, "uColor"), 0.2f, 0.8f, 0.2f, 1.f);

	Core::System::CameraManager& CM = Core::System::CameraManager::GetInstance();

	Mat4x4 ViewProj = CM.GetProjectionMatrix();
	if (CM.HasCamera())
		ViewProj = ViewProj * CM.GetCamera()->GetViewMatrix();

	glProgramUniformMatrix4fv(shaderCollider, glGetUniformLocation(shaderCollider, "uVP"), 1, GL_FALSE, ViewProj.m_mat.data());

	Transform* CurrentTransform = nullptr;
	if (currentGo->TryGetComponent(&CurrentTransform))
	{
		Vec3 scale;

		switch (m_shape)
		{
		case(Shape::BOX):
			scale = m_boxScale;
			break;
		case(Shape::CAPSULE):
			scale.m_x = m_capsuleRadius;
			scale.m_y = m_capsuleHalfHeight + m_capsuleRadius;
			scale.m_z = m_capsuleRadius;
			break;
		case(Shape::SPHERE):
			scale = m_sphereRadius;
			break;
		default:
			break;
		}

		PxTransform currentPhysicTransform = m_actor->getGlobalPose();
		Quaternion rot = Quaternion(currentPhysicTransform.q.x, currentPhysicTransform.q.y, currentPhysicTransform.q.z, currentPhysicTransform.q.w);
		Mat4x4 Model = Quaternion::TRSQuat(Vec3(currentPhysicTransform.p.x, currentPhysicTransform.p.y, currentPhysicTransform.p.z) + m_colliderOffset,
			rot,
			scale);

		/*Quaternion rot = CurrentTransform->GetGlobalRotation();
		Mat4x4 Model = Quaternion::TRSQuat(CurrentTransform->GetGlobalPosition() + m_colliderOffset,
										   rot,
										   scale);/**/
		glProgramUniformMatrix4fv(shaderCollider, glGetUniformLocation(shaderCollider, "uModel"), 1, GL_FALSE, Model.m_mat.data());
	}

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data) / 3);

	glBindVertexArray(0u);
	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ECS::RigidBody::EditorUpdate(Core::GameObject* p_gameObject)
{
	UpdateColliderPos(p_gameObject);
}

void ECS::RigidBody::ShowEditorControl(const unsigned int p_indexComponent)
{
	ShowBooleans(p_indexComponent);

	ShowShapeOptions(p_indexComponent);

	ShowConstraints(p_indexComponent);

	ShowShapeScales(p_indexComponent);
}

void ECS::RigidBody::Save(tinyxml2::XMLElement* ComponentXml)
{
	tinyxml2::XMLElement* boxScaleXML = ComponentXml->InsertNewChildElement("boxScale");
	boxScaleXML->SetAttribute("X", m_boxScale.m_x);
	boxScaleXML->SetAttribute("Y", m_boxScale.m_y);
	boxScaleXML->SetAttribute("Z", m_boxScale.m_z);

	tinyxml2::XMLElement* capsuleScaleXML = ComponentXml->InsertNewChildElement("capsuleScale");
	capsuleScaleXML->SetAttribute("halfHeight", m_capsuleHalfHeight);
	capsuleScaleXML->SetAttribute("capsuleRadius", m_capsuleRadius);

	tinyxml2::XMLElement* sphereScaleXML = ComponentXml->InsertNewChildElement("sphereScale");
	sphereScaleXML->SetAttribute("sphereRadius", m_sphereRadius);

	tinyxml2::XMLElement* booleansXML = ComponentXml->InsertNewChildElement("booleans");
	booleansXML->SetAttribute("drawCollider", m_drawCollider);
	booleansXML->SetAttribute("isTrigger", m_isTrigger);
	booleansXML->SetAttribute("isKinematic", m_isKinematic);
	booleansXML->SetAttribute("useGravity", m_useGravity);

	tinyxml2::XMLElement* offsetXML = ComponentXml->InsertNewChildElement("offset");
	offsetXML->SetAttribute("offsetX", m_colliderOffset.m_x);
	offsetXML->SetAttribute("offsetY", m_colliderOffset.m_y);
	offsetXML->SetAttribute("offsetZ", m_colliderOffset.m_z);

	tinyxml2::XMLElement* constraintsXML = ComponentXml->InsertNewChildElement("constraints");
	constraintsXML->SetAttribute("translationX", m_lockTranslationAxisX);
	constraintsXML->SetAttribute("translationY", m_lockTranslationAxisY);
	constraintsXML->SetAttribute("translationZ", m_lockTranslationAxisZ);

	constraintsXML->SetAttribute("rotationX", m_lockRotationAxisX);
	constraintsXML->SetAttribute("rotationY", m_lockRotationAxisY);
	constraintsXML->SetAttribute("rotationZ", m_lockRotationAxisZ);

	tinyxml2::XMLElement* shapeXML = ComponentXml->InsertNewChildElement("shape");
	shapeXML->SetAttribute("type", m_shape);
}
#endif // EDITOR_MODE

/////////////////////////////////////////////////////  Private methods  /////////////////////////////////////////////////////

void ECS::RigidBody::SetCollider()
{
	PxShape* shape;
	m_actor->getShapes(&shape, m_actor->getNbShapes());
	m_colliderLocalTr = m_actor->getGlobalPose();
	m_actor->detachShape(shape[0]);

	switch (m_shape)
	{
	case Shape::BOX:
		PxRigidActorExt::createExclusiveShape(*m_actor,
			PxBoxGeometry(m_boxScale.m_x, m_boxScale.m_y, m_boxScale.m_z),
			*m_material);
		shape[0].setLocalPose(PxTransform({ m_colliderOffset.m_x, m_colliderOffset.m_y, m_colliderOffset.m_z }));
		break;
	case Shape::CAPSULE:
		PxRigidActorExt::createExclusiveShape(*m_actor,
			PxCapsuleGeometry(m_capsuleRadius, m_capsuleHalfHeight),
			*m_material);
		shape[0].setLocalPose(PxTransform({ m_colliderOffset.m_x, m_colliderOffset.m_y, m_colliderOffset.m_z }, PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
		break;
	case Shape::SPHERE:
		PxRigidActorExt::createExclusiveShape(*m_actor,
			PxSphereGeometry(m_sphereRadius),
			*m_material);
		shape[0].setLocalPose(PxTransform({ m_colliderOffset.m_x, m_colliderOffset.m_y, m_colliderOffset.m_z }));
		break;
	default:
		break;
	}

}

void ECS::RigidBody::SetConstraints()
{
	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, m_lockRotationAxisX);
	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, m_lockRotationAxisY);
	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, m_lockRotationAxisZ);

	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, m_lockTranslationAxisX);
	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, m_lockTranslationAxisY);
	m_actor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, m_lockTranslationAxisZ);

	if (m_actor->isSleeping() && !m_isKinematic)
		m_actor->wakeUp();
}

void ECS::RigidBody::SetKinematic()
{
	m_actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, m_isKinematic);
	if (!m_isKinematic)
		m_actor->wakeUp();
}

void ECS::RigidBody::SetTrigger()
{
	PxShape* shape;
	m_actor->getShapes(&shape, 1);
	if (m_isTrigger)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
	}
	else
	{
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	}
}

void ECS::RigidBody::SetUsingGravity()
{
	m_actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !m_useGravity);
	if (m_useGravity && !m_isKinematic)
		m_actor->wakeUp();
}

/////////////////////////////////////////////////////  IMGUI constrols  /////////////////////////////////////////////////////

#ifdef MODE_EDITOR
void ECS::RigidBody::ShowBooleans(const unsigned int p_indexComponent)
{
	ImGui::Text("Draw collider : ");
	ImGui::SameLine();
	ImGui::Checkbox(("##drawCollider-" + std::to_string(p_indexComponent)).c_str(), &m_drawCollider);

	ImGui::Text("Is trigger : ");
	ImGui::SameLine();
	if (ImGui::Checkbox(("##isColliderTrigger-" + std::to_string(p_indexComponent)).c_str(), &m_isTrigger))
		SetTrigger();

	ImGui::Text("Is kinematic : ");
	ImGui::SameLine();
	if (ImGui::Checkbox(("##isColliderKinematic-" + std::to_string(p_indexComponent)).c_str(), &m_isKinematic))
		SetKinematic();

	ImGui::Text("Use gravity : ");
	ImGui::SameLine();
	if (ImGui::Checkbox(("##isColliderFalling-" + std::to_string(p_indexComponent)).c_str(), &m_useGravity))
		SetUsingGravity();
}

void ECS::RigidBody::ShowConstraints(const unsigned int p_indexComponent)
{

	ImGui::Text("Constraints : ");
	ImGui::Text("Translation : "); //Linear Constraints
	if (ImGui::Checkbox(("X##TranslationX-" + std::to_string(p_indexComponent)).c_str(), &m_lockTranslationAxisX))
		SetConstraints();

	ImGui::SameLine();
	if (ImGui::Checkbox(("Y##TranslationY-" + std::to_string(p_indexComponent)).c_str(), &m_lockTranslationAxisY))
		SetConstraints();

	ImGui::SameLine();
	if (ImGui::Checkbox(("Z##TranslationZ-" + std::to_string(p_indexComponent)).c_str(), &m_lockTranslationAxisZ))
		SetConstraints();

	ImGui::Text("Rotation : "); //Angular Constraints
	if (ImGui::Checkbox(("X##RotationX-" + std::to_string(p_indexComponent)).c_str(), &m_lockRotationAxisX))
		SetConstraints();

	ImGui::SameLine();
	if (ImGui::Checkbox(("Y##RotationY-" + std::to_string(p_indexComponent)).c_str(), &m_lockRotationAxisY))
		SetConstraints();

	ImGui::SameLine();

	if (ImGui::Checkbox(("Z##RotationZ-" + std::to_string(p_indexComponent)).c_str(), &m_lockRotationAxisZ))
		SetConstraints();
}

void ECS::RigidBody::ShowShapeOptions(const unsigned int p_indexComponent)
{
	ImGui::Text("Collider offset : ");
	if (ImGui::DragFloat3(("##ColliderOffset-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_colliderOffset, 0.01f, FLT_MAX))
	{
		m_newOffset = true;
		UpdateOffset();
	}
	ImGui::Text("Collider shape : ");
	if (ImGui::RadioButton(("Box##BoxShape-" + std::to_string(p_indexComponent)).c_str(), m_shape == Shape::BOX))
	{
		m_shape = Shape::BOX;
		SetCollider();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(("Capsule##CapsuleShape-" + std::to_string(p_indexComponent)).c_str(), m_shape == Shape::CAPSULE))
	{
		m_shape = Shape::CAPSULE;
		SetCollider();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton(("Sphere##SphereShape-" + std::to_string(p_indexComponent)).c_str(), m_shape == Shape::SPHERE))
	{
		m_shape = Shape::SPHERE;
		SetCollider();
	}
}

void ECS::RigidBody::ShowShapeScales(const unsigned int p_indexComponent)
{
	switch (m_shape)
	{
	case Shape::BOX:
		ImGui::Text("Box scale : ");
		if (ImGui::DragFloat3(("scale##BoxScale-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_boxScale, 0.01f, 0.01f, FLT_MAX))
			SetCollider();
		break;
	case Shape::CAPSULE:
		ImGui::Text("Capsule half height : ");
		if (ImGui::DragFloat(("scale##CapsuleHalfHeight-" + std::to_string(p_indexComponent)).c_str(), &m_capsuleHalfHeight, 0.01f, 0.01f, FLT_MAX))
			SetCollider();
		ImGui::Text("Capsule radius : ");
		if (ImGui::DragFloat(("scale##CapsuleRadius-" + std::to_string(p_indexComponent)).c_str(), &m_capsuleRadius, 0.01f, 0.01f, FLT_MAX))
			SetCollider();
		break;
	case Shape::SPHERE:
		ImGui::Text("Sphere radius : ");
		if (ImGui::DragFloat(("scale##CapsuleRadius-" + std::to_string(p_indexComponent)).c_str(), &m_sphereRadius, 0.01f, 0.01f, FLT_MAX))
			SetCollider();
		break;
	default:
		break;
	}
}
#endif // EDITOR_MODE