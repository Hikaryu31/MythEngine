#include "Core/System/LightManager.h"

SINGLETON_SETUP(Core::System::LightManager)

Core::System::LightManager::LightManager()
{
	MYTH_ASSERT(singleton == nullptr);
	singleton = this;
}

void Core::System::LightManager::RegisterPointLight(ECS::PointLight* p_lightptr)
{
	MYTH_ASSERT(singleton != nullptr);

	singleton->m_pointLights.push_back(p_lightptr);
}

void Core::System::LightManager::UnregisterPointLight(ECS::PointLight* p_lightptr)
{
	MYTH_ASSERT(singleton != nullptr);

	for (unsigned int i = 0u; i < singleton->m_pointLights.size(); i++)
		if (singleton->m_pointLights[i] == p_lightptr)
		{
			singleton->m_pointLights.erase(singleton->m_pointLights.begin() + i);
			i--;
		}
}

void Core::System::LightManager::SelectPointLight(ECS::DirectionLight* p_lightptr, const bool p_setSelected)
{
	MYTH_ASSERT(singleton != nullptr);

	if (singleton->m_directionLights != nullptr && p_setSelected)
		singleton->m_directionLights->SetSelected(false);

	singleton->m_directionLights = p_lightptr;

	if (singleton->m_directionLights != nullptr && p_setSelected)
		singleton->m_directionLights->SetSelected(true);
}
