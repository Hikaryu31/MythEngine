#pragma once

#include "define.h"
#include "maths/Vec3.hpp"
#include "Core/BaseSingleton.h"

namespace Core
{
	namespace Debug
	{
		struct sDrawLineData
		{
			Vec3 Start;
			Vec3 SColor;
			Vec3 End;
			Vec3 EColor;
		};

		class Draw : public BaseSingleton<Draw>
		{
		public:
			Draw() { MYTH_ASSERT(singleton == nullptr); singleton = this; }
			~Draw() { singleton = nullptr; }

			static void Line(const Vec3& p_start, const Vec3& p_end, const Vec3& p_color = { 0.f, 0.f, 0.f });

			void Render();

		private:
			std::vector<sDrawLineData> m_lineList;

			unsigned int m_shaderLine = 0u;
		};
	}
}