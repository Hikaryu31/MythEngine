#include "Resources/Animation.h"
#include "maths/Quaternion.hpp"

REGISTER_RESOURCE_CPP(Resources::Animation)

Resources::Animation::Animation(const std::string& p_name) : Resource(p_name)
{
	m_scene = m_importer.ReadFile(p_name, aiProcess_Triangulate);
	MYTH_ASSERT(m_scene && m_scene->mRootNode);

	aiAnimation* animation = m_scene->mAnimations[m_index];
	m_duration = (float)animation->mDuration;
	m_ticksPerSecond = (int)animation->mTicksPerSecond;
	m_numberAnim = m_scene->mNumAnimations;
}

void Resources::Animation::LoadResource()
{
	if (m_loaded)
		return;

	if (m_scene->HasAnimations())
		for (unsigned int i = 0; i < m_scene->mAnimations[m_index]->mNumChannels; i++)
			m_nodeAnimMap.insert({ m_scene->mAnimations[m_index]->mChannels[i]->mNodeName.C_Str(), m_scene->mAnimations[m_index]->mChannels[i] });

	m_loaded = true;
}

void Resources::Animation::UnloadResource()
{
	if (!m_loaded)
		return;

	m_nodeAnimMap.clear();

	m_loaded = false;
}

#ifdef MODE_EDITOR
void Resources::Animation::ShowEditorControl()
{
	ImGui::SliderInt("index of animation", &m_index, 0, m_numberAnim - 1);
}
#endif // MODE_EDITOR
