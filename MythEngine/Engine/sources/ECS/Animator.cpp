#include "ECS/Animator.h"
#include "ECS/ModelInstance.h"
#include "Core/GameObject.h"
#include "Core/System/Application.h"
#include "maths/Quaternion.hpp"

REGISTER_COMPONENT_CPP(ECS::Animator)

ECS::Animator::Animator() : Component()
{
	m_boneTransforms.reserve(100);

	for (int i = 0; i < 100; i++)
		m_boneTransforms.push_back(Mat4x4::Identity);

	m_AnimationEndEvent = Core::System::EventManager::CreateNewEvent();
}

ECS::Animator::~Animator()
{
	m_AnimationEndEvent = nullptr;
}

void ECS::Animator::Start(Core::GameObject* p_gameObject)
{
	if (m_currentAnimation)
	{
		ECS::ModelInstance* model = nullptr;

		if (p_gameObject->TryGetComponent(&model))
		{
			if (m_boneInfoMap.empty() && model->Model != nullptr)
				m_boneInfoMap = model->Model->GetBoneMap();

			model->m_hasAnim = true;

			GetBoneTransforms(model->m_boneTransforms, model->GetWorldMatrix());
		}
	}
}

void ECS::Animator::Update(Core::GameObject* p_gameObject)
{
	if (m_currentAnimation)
	{
		ECS::ModelInstance* model = nullptr;

		if (p_gameObject->TryGetComponent(&model))
		{
			if (m_boneInfoMap.empty() && model->Model != nullptr)
				m_boneInfoMap = model->Model->GetBoneMap();

			model->m_hasAnim = true;

			if (m_play && Core::System::Application::GetInstance().PlayAnimation())
				m_currentTime += m_currentAnimation->m_ticksPerSecond * Core::System::Time::DeltaTime();

			if (m_currentTime >= m_currentAnimation->m_duration)
			{
				m_currentTime = fmod(m_currentTime, m_currentAnimation->m_duration);

				m_AnimationEndEvent->notify(this);
			}

			GetBoneTransforms(model->m_boneTransforms, model->GetWorldMatrix());
		}
	}
}

void ECS::Animator::Load(tinyxml2::XMLElement* ComponentXml)
{
	const char* currentAnimationName = ComponentXml->Attribute("animation");

	if (currentAnimationName != nullptr)
	{
		Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

		if (!rm.TryGetAndLoadResource(currentAnimationName, &m_currentAnimation))
		{
			m_currentAnimation = std::make_shared<Resources::Animation>(currentAnimationName);
			rm.RegisterAndLoadResource(m_currentAnimation);
		}
	}

	m_currentTime = ComponentXml->FloatAttribute("time");
	m_play = ComponentXml->BoolAttribute("play", true);
}

void ECS::Animator::Play(const float p_at)
{
	m_play = true;

	if (p_at >= 0.f)
		m_currentTime = p_at;
}

void ECS::Animator::Play(std::shared_ptr<Resources::Animation> p_animation, const float p_at)
{
	m_previousAnimation = m_currentAnimation;
	m_currentAnimation = p_animation;
	m_play = true;

	if (p_at >= 0.f)
		m_currentTime = p_at;
}

void ECS::Animator::Stop()
{
	m_play = true;
	m_currentTime = 0.f;
}

unsigned int ECS::Animator::FindPosition(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	for (unsigned int i = 0; i < p_nodeAnim->mNumPositionKeys - 1; i++)
	{
		float t = (float)p_nodeAnim->mPositionKeys[i + 1].mTime;
		if (p_animationTimeTicks < t)
			return i;
	}
	return 0;
}

unsigned int ECS::Animator::FindRotation(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	for (unsigned int i = 0; i < p_nodeAnim->mNumRotationKeys - 1; i++)
	{
		float t = (float)p_nodeAnim->mRotationKeys[i + 1].mTime;
		if (p_animationTimeTicks < t)
			return i;
	}
	return 0;
}

unsigned int ECS::Animator::FindScaling(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	MYTH_ASSERT(p_nodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < p_nodeAnim->mNumScalingKeys - 1; i++)
	{
		float t = (float)p_nodeAnim->mScalingKeys[i + 1].mTime;
		if (p_animationTimeTicks < t)
			return i;
	}
	return 0;
}

void ECS::Animator::CalcInterpolatedPosition(aiVector3D& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	if (p_nodeAnim->mNumPositionKeys == 1)
	{
		p_out = p_nodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int positionIndex = FindPosition(p_animationTimeTicks, p_nodeAnim);
	unsigned int nextPositionIndex = positionIndex + 1;
	MYTH_ASSERT(nextPositionIndex < p_nodeAnim->mNumPositionKeys);

	float t1 = (float)p_nodeAnim->mPositionKeys[positionIndex].mTime;
	float t2 = (float)p_nodeAnim->mPositionKeys[nextPositionIndex].mTime;
	float deltaTime = t2 - t1;

	float factor = (p_animationTimeTicks - t1) / deltaTime;
	MYTH_ASSERT(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = p_nodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D& end = p_nodeAnim->mPositionKeys[nextPositionIndex].mValue;
	aiVector3D delta = end - start;
	p_out = start + factor * delta;
}

void ECS::Animator::CalcInterpolatedRotation(aiQuaternion& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	if (p_nodeAnim->mNumRotationKeys == 1)
	{
		p_out = p_nodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int rotationIndex = FindRotation(p_animationTimeTicks, p_nodeAnim);
	unsigned int nextRotationIndex = rotationIndex + 1;
	MYTH_ASSERT(nextRotationIndex < p_nodeAnim->mNumRotationKeys);

	float t1 = (float)p_nodeAnim->mRotationKeys[rotationIndex].mTime;
	float t2 = (float)p_nodeAnim->mRotationKeys[nextRotationIndex].mTime;
	float deltaTime = t2 - t1;

	float factor = (p_animationTimeTicks - t1) / deltaTime;
	MYTH_ASSERT(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion& start = p_nodeAnim->mRotationKeys[rotationIndex].mValue;
	const aiQuaternion& end = p_nodeAnim->mRotationKeys[nextRotationIndex].mValue;
	aiQuaternion::Interpolate(p_out, start, end, factor);
	p_out = start;
	p_out.Normalize();
}

void ECS::Animator::CalcInterpolatedScaling(aiVector3D& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim)
{
	if (p_nodeAnim->mNumScalingKeys == 1)
	{
		p_out = p_nodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int scalingIndex = FindScaling(p_animationTimeTicks, p_nodeAnim);
	unsigned int nextScalingIndex = scalingIndex + 1;
	MYTH_ASSERT(nextScalingIndex < p_nodeAnim->mNumScalingKeys);

	float t1 = (float)p_nodeAnim->mScalingKeys[scalingIndex].mTime;
	float t2 = (float)p_nodeAnim->mScalingKeys[nextScalingIndex].mTime;
	float deltaTime = t2 - t1;

	float factor = (p_animationTimeTicks - t1) / deltaTime;
	MYTH_ASSERT(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = p_nodeAnim->mScalingKeys[scalingIndex].mValue;
	const aiVector3D& end = p_nodeAnim->mScalingKeys[nextScalingIndex].mValue;
	aiVector3D delta = end - start;
	p_out = start + factor * delta;
}

void ECS::Animator::CalculateBoneTransforms(const aiNode* p_node, const Mat4x4& p_parentTransform)
{
	std::string nodeName(p_node->mName.C_Str());
	Mat4x4 nodeTransform = Resources::AiMatrixToMat4x4(p_node->mTransformation);
	const aiNodeAnim* nodeAnim = m_currentAnimation->m_nodeAnimMap[nodeName];

	if (nodeAnim)
	{
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, m_currentTime, nodeAnim);
		Mat4x4 scalingMatrix = Mat4x4::Scale({ scaling.x, scaling.y, scaling.z });

		aiQuaternion rotation;
		CalcInterpolatedRotation(rotation, m_currentTime, nodeAnim);
		Quaternion rotationQ(rotation.x, rotation.y, rotation.z, rotation.w);
		Mat4x4 rotationMatrix = rotationQ.GetRotationMatrix();

		aiVector3D translation;
		CalcInterpolatedPosition(translation, m_currentTime, nodeAnim);
		Mat4x4 translationMatrix = Mat4x4::Translate({ translation.x, translation.y, translation.z });

		nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}

	Mat4x4 globalTransform = p_parentTransform * nodeTransform;
	if (m_boneInfoMap.find(nodeName) != m_boneInfoMap.end())
	{
		int index = m_boneInfoMap[nodeName].id;
		m_boneTransforms[index] = globalTransform * m_boneInfoMap[nodeName].offsetMatrix;
	}

	for (unsigned int i = 0; i < p_node->mNumChildren; i++)
		CalculateBoneTransforms(p_node->mChildren[i], globalTransform);
}

void ECS::Animator::CalculateBlendedBoneTransforms(const aiNode* p_node, const aiNode* p_nodePreviousAnim, const Mat4x4& p_parentTransform)
{
	std::string nodeName(p_node->mName.C_Str());
	Mat4x4 nodeTransform = Resources::AiMatrixToMat4x4(p_node->mTransformation);
	const aiNodeAnim* nodeAnim = m_currentAnimation->m_nodeAnimMap[nodeName];

	if (nodeAnim)
	{
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, m_currentTime, nodeAnim);
		Mat4x4 scalingMatrix = Mat4x4::Scale({ scaling.x, scaling.y, scaling.z });

		aiQuaternion rotation;
		CalcInterpolatedRotation(rotation, m_currentTime, nodeAnim);
		Quaternion rotationQ(rotation.x, rotation.y, rotation.z, rotation.w);
		Mat4x4 rotationMatrix = rotationQ.GetRotationMatrix();

		aiVector3D translation;
		CalcInterpolatedPosition(translation, m_currentTime, nodeAnim);
		Mat4x4 translationMatrix = Mat4x4::Translate({ translation.x, translation.y, translation.z });

		nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}

	std::string previousAnimNodeName(p_nodePreviousAnim->mName.C_Str());
	Mat4x4 previousAnimNodeTransform = Resources::AiMatrixToMat4x4(p_nodePreviousAnim->mTransformation);
	nodeAnim = m_previousAnimation->m_nodeAnimMap[previousAnimNodeName];

	if (nodeAnim)
	{
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, m_previousAnimTime, nodeAnim);
		Mat4x4 scalingMatrix = Mat4x4::Scale({ scaling.x, scaling.y, scaling.z });

		aiQuaternion rotation;
		CalcInterpolatedRotation(rotation, m_previousAnimTime, nodeAnim);
		Quaternion rotationQ(rotation.x, rotation.y, rotation.z, rotation.w);
		Mat4x4 rotationMatrix = rotationQ.GetRotationMatrix();

		aiVector3D translation;
		CalcInterpolatedPosition(translation, m_previousAnimTime, nodeAnim);
		Mat4x4 translationMatrix = Mat4x4::Translate({ translation.x, translation.y, translation.z });

		previousAnimNodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}
	
	const Quaternion rot0 = Quaternion::GetQuatFromRotMat(previousAnimNodeTransform);
	const Quaternion rot1 = Quaternion::GetQuatFromRotMat(nodeTransform);
	Quaternion blendedRot = Quaternion::Slerp(rot0, rot1, m_blendFactor);
	Mat4x4 blendedMatrix = blendedRot.GetRotationMatrix();
	blendedMatrix.SetColumn(3, (1.f - m_blendFactor) * nodeTransform[3] + previousAnimNodeTransform[3] * m_blendFactor);

	Mat4x4 globalTransform = p_parentTransform * blendedMatrix;
	if (m_boneInfoMap.find(nodeName) != m_boneInfoMap.end())
	{
		int index = m_boneInfoMap[nodeName].id;
		m_boneTransforms[index] = globalTransform * m_boneInfoMap[nodeName].offsetMatrix;
	}

	for (unsigned int i = 0; i < p_node->mNumChildren; i++)
		CalculateBlendedBoneTransforms(p_node->mChildren[i], p_nodePreviousAnim->mChildren[i], globalTransform);
}

void ECS::Animator::GetBoneTransforms(std::vector<Mat4x4>& p_transforms, const Mat4x4& p_worldMatrix)
{
	if (m_currentAnimation == nullptr)
		return;

	if (m_previousAnimation == nullptr)
		CalculateBoneTransforms(m_currentAnimation->GetAssimpScene()->mRootNode, p_worldMatrix);

	else
	{
		m_blendFactor += 0.1f;
		m_blendFactor = Maths::Clamp(m_blendFactor, 0.f, 1.f);

		float a = 1.f;
		float b = m_previousAnimation->m_duration / m_currentAnimation->m_duration;
		const float animSpeedMultiplier1 = (1.f - m_blendFactor) * a + b * m_blendFactor;

		a = m_currentAnimation->m_duration / m_previousAnimation->m_duration;
		b = 1.f;
		const float animSpeedMultiplier2 = (1.f - m_blendFactor) * a + b * m_blendFactor;

		m_previousAnimTime = 0.f;
		m_previousAnimTime += m_previousAnimation->m_ticksPerSecond * Core::System::Time::DeltaTime() * animSpeedMultiplier1;
		m_previousAnimTime = fmod(m_previousAnimTime, m_previousAnimation->m_duration);

		m_currentTime *= animSpeedMultiplier2;
		m_currentTime = fmod(m_currentTime, m_currentAnimation->m_duration);
		CalculateBlendedBoneTransforms(m_currentAnimation->GetAssimpScene()->mRootNode,
										m_previousAnimation->GetAssimpScene()->mRootNode, p_worldMatrix);
		if (m_blendFactor == 1.f)
		{
			m_blendFactor = 0;
			m_previousAnimation = nullptr;
		}
	}

	p_transforms.resize(m_boneTransforms.size());

	for (int i = 0; i < m_boneTransforms.size(); i++)
		p_transforms[i] = m_boneTransforms[i];
}

#ifdef MODE_EDITOR
void ECS::Animator::AnimationCreator()
{
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Animation Creator", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string newAnimationPath = "";
		ImGui::Text("Animation path :");
		ImGui::SameLine();
		ImGui::InputText("##AnimationPath_AnimationCreator", &newAnimationPath);

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)) && !newAnimationPath.empty())
		{
			Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();

			if (!rm.TryGetAndLoadResource<Resources::Animation>(newAnimationPath, &m_currentAnimation))
			{
				m_previousAnimation = m_currentAnimation;
				m_currentAnimation = std::make_shared<Resources::Animation>(newAnimationPath);
				rm.RegisterAndLoadResource(m_currentAnimation);
			}
			else
				MYTH_WARNING("Animation '%s' already exist", newAnimationPath.c_str());

			newAnimationPath = "";
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { newAnimationPath = ""; ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void ECS::Animator::ShowEditorControl(const unsigned int p_indexComponent)
{
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.45f);

	static std::string AnimationlName = "NONE";
	AnimationlName = (m_currentAnimation == nullptr ? "NONE" : m_currentAnimation->GetName());
	ImGui::Text("Animation :");
	ImGui::SameLine();
	ImGui::InputText(("##AnimationOfmodelInstance_" + std::to_string(p_indexComponent)).c_str(), &AnimationlName, ImGuiInputTextFlags_ReadOnly);
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_RESOURCE"))
			Core::System::Application::GetInstance().GetResourcesManager().TryGetAndLoadResource(std::string((const char*)payload->Data), &m_currentAnimation);
		ImGui::EndDragDropTarget();
	}
	ImGui::SameLine();
	if (ImGui::Button(("X##AnimationModelInstance_" + std::to_string(p_indexComponent)).c_str()))
		m_currentAnimation = nullptr;
	ImGui::SameLine();
	if (ImGui::Button(("Create animation##modelInstance_" + std::to_string(p_indexComponent)).c_str()))
		ImGui::OpenPopup("Animation Creator");
	AnimationCreator();

	ImGui::PopItemWidth();

	ImGui::Spacing();

	if (m_currentAnimation != nullptr)
	{
		if (m_play)
		{
			if (ImGui::Button(("Pause##AnimationModelInstance_" + std::to_string(p_indexComponent)).c_str()))
				Pause();
		}
		else
		{
			if (ImGui::Button(("Play ##AnimationModelInstance_" + std::to_string(p_indexComponent)).c_str()))
				Play();
		}

		ImGui::SameLine();
		ImGui::SliderFloat(("##SliderModelInstance_" + std::to_string(p_indexComponent)).c_str(), &m_currentTime, 0.f, m_currentAnimation->m_duration);
	}
}

void ECS::Animator::Save(tinyxml2::XMLElement* ComponentXml)
{
	if (m_currentAnimation != nullptr)
		ComponentXml->SetAttribute("animation", m_currentAnimation->GetName().c_str());

	ComponentXml->SetAttribute("time", m_currentTime);
	ComponentXml->SetAttribute("play", m_play);
}
#endif // MODE_EDITOR