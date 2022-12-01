#pragma once

#include "define.h"
#include "Core/BaseSingleton.h"

namespace Core
{
	namespace System
	{
		class Time : public BaseSingleton<Time>
		{
		public:
			Time();
			~Time() { singleton = nullptr; }

			static inline float  DeltaTime() { MYTH_ASSERT(singleton != nullptr); return singleton->m_deltaTime * singleton->m_timeScale; }
			static inline float  FixedDeltaTime() { MYTH_ASSERT(singleton != nullptr); return singleton->m_fixedDeltaTime * singleton->m_timeScale; }
			static inline int    Fixing() { MYTH_ASSERT(singleton != nullptr); return (singleton->m_fixCount == 0 ? 1 : singleton->m_fixCount); }
			static inline double GetTime() { MYTH_ASSERT(singleton != nullptr); return singleton->m_lastTime; }
		//	static inline int   Fixing() { MYTH_ASSERT(singleton != nullptr); return singleton->m_fixCount; }

			inline float& TimeScale() { return m_timeScale; }
			inline float& TargetFrameRate() { return m_targetFrameRate; }

			void ResetLastTime();
			void Update();

		private:

			double m_lastTime;
			float  m_timeScale;
			float  m_deltaTime;
			float  m_targetFrameRate;
			float  m_fixedDeltaTime;
			int    m_fixCount;
		};
	}
}

#define MYTH_TIME Core::System::Time::GetInstance()