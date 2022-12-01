#pragma once

#include "define.h"

#include <atomic>

namespace Core
{
	namespace System
	{
		class Random
		{
		public:
			static void Initialization();

			static int GetRandomInt(const int p_min, const int p_max);
			static int GetRandomInt(const int p_max) { return GetRandomInt(0, p_max); }

			static float GetRandomFloat(const float p_min, const float p_max);
			static float GetRandomFloat(const float p_max) { return GetRandomFloat(0, p_max); }

		private:
			static std::atomic_flag m_lockRandom;
		};
	}
}