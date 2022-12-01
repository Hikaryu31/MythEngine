#pragma once

#include "ECS/Component.h"
#include "Resources/Animation.h"

#include "event_notifier.hpp"

namespace ECS
{
	class Animator : public Component, public GameplayComponent
	{
	public :
		Animator();
		~Animator();

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_GAMEPLAY)

		std::shared_ptr<Resources::Animation> m_currentAnimation = nullptr;
		std::vector<Mat4x4> m_boneTransforms;

		void Start(Core::GameObject* p_gameObject) override;
		void Update(Core::GameObject* p_gameObject) override;
		void Load(tinyxml2::XMLElement* ComponentXml) override;

		void Play(const float p_at = -1.f);
		void Play(std::shared_ptr<Resources::Animation> p_animation, const float p_at = 0.f);
		void Pause() { m_play = false; }
		void Stop();
		void SetCurrentTime(const float p_at) { m_currentTime = p_at; }

		stypox::EventNotifier& GetAnimationEndEvent() { return *m_AnimationEndEvent; }

		void CalculateBoneTransforms(const aiNode* p_node, const Mat4x4& p_parentTransform);
		void CalculateBlendedBoneTransforms(const aiNode* p_node, const aiNode* p_nodePreviousAnim, const Mat4x4& p_parentTransform);
		void GetBoneTransforms(std::vector<Mat4x4>& p_transforms, const Mat4x4& p_worldMatrix);

#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { Update(p_gameObject); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
		void AnimationCreator();
#endif // MODE_EDITOR

	private :
		float m_currentTime = 0.f;
		float m_previousAnimTime = 0.f;
		float m_blendFactor = 0.f;
		bool  m_play = true;
		std::shared_ptr<Resources::Animation> m_previousAnimation = nullptr;

		std::shared_ptr<stypox::EventNotifier> m_AnimationEndEvent = nullptr;

		std::unordered_map<std::string, Resources::BoneInfo> m_boneInfoMap;

		void CalcInterpolatedPosition(aiVector3D& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);
		void CalcInterpolatedScaling(aiVector3D& p_out, float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);

		unsigned int FindPosition(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);
		unsigned int FindRotation(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);
		unsigned int FindScaling(float p_animationTimeTicks, const aiNodeAnim* p_nodeAnim);
	};
}