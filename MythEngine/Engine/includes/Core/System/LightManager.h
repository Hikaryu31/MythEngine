#pragma once

#include "define.h"

#include "Core/BaseSingleton.h"

#include "ECS/Light.h"
#include "maths/Vec3.hpp"

#include <vector>

namespace Core
{
	namespace System
	{
		class LightManager : public BaseSingleton<LightManager>
		{
		public:
			LightManager();
			~LightManager() { singleton = nullptr; }

			static void RegisterPointLight(ECS::PointLight* p_lightptr);
			static void UnregisterPointLight(ECS::PointLight* p_lightptr);
			static const std::vector<ECS::PointLight*>& GetPointLights() { MYTH_ASSERT(singleton != nullptr); return singleton->m_pointLights; }

			static void SelectPointLight(ECS::DirectionLight* p_lightptr, const bool p_setSelected = true);
			static const ECS::DirectionLight* GetDirectionLight() { MYTH_ASSERT(singleton != nullptr); return singleton->m_directionLights; }

		private:
			std::vector<ECS::PointLight*> m_pointLights;
			ECS::DirectionLight* m_directionLights = nullptr;
		};
	}
}