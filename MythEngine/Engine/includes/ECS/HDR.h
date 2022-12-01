#pragma once
#include "ECS/Component.h"
#include "Resources/Shader.h"

namespace ECS
{
	class HDR
	{
	public :
		HDR();
		~HDR() { glDeleteFramebuffers(1, &m_hdrFBO); glDeleteFramebuffers(2, m_pingpongFBO); };

		std::shared_ptr<Resources::HDRShader> m_hdrShader = nullptr;
		std::shared_ptr<Resources::Shader> m_blurShader = nullptr;
		unsigned int m_hdrFBO = 0;
		unsigned int m_pingpongFBO[2];

		unsigned int m_colorBuffer[2];
		unsigned int m_pingpongColorBuffer[2];

		unsigned int quadVAO = 0;
		unsigned int quadVBO = 0;
		unsigned int FBOwidth = 0;
		unsigned int FBOheight = 0;
		bool horizontal = true;

		void Update();
		void Draw();
		void renderQuad();
	};
}