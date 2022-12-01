#include "ECS/HDR.h"
#include "Core/System/Application.h"
#include "Core/System/CameraManager.h"

ECS::HDR::HDR()
{
	Resources::ResourcesManager& rm = Core::System::Application::GetInstance().GetResourcesManager();
	if (!rm.TryGetAndLoadResource<Resources::HDRShader>("hdrShader", &m_hdrShader))
	{
		m_hdrShader = std::make_shared<Resources::HDRShader>("hdrShader");
		rm.RegisterAndLoadResource(m_hdrShader);
	}

	if (!rm.TryGetAndLoadResource<Resources::Shader>("blurShader", &m_blurShader))
	{
		m_blurShader = std::make_shared<Resources::Shader>("blurShader");
		rm.RegisterAndLoadResource(m_blurShader);
	}

	Core::System::CameraManager& cam = Core::System::CameraManager::GetInstance();
	FBOwidth = cam.WindowWidth;
	FBOheight = cam.WindowHeight;

	// FBO
	glGenFramebuffers(1, &m_hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);

	// CBO
	glGenTextures(2, m_colorBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_colorBuffer[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, FBOwidth, FBOheight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_colorBuffer[i], 0);
	}

	// RBO
	unsigned int renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FBOwidth, FBOheight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Check errors
	auto m_hdrFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (m_hdrFBOStatus != GL_FRAMEBUFFER_COMPLETE)
		MYTH_ERROR("Framebuffer error: %u", m_hdrFBOStatus);

	// FBO
	glGenFramebuffers(2, m_pingpongFBO);

	// 2nd FBO
	glGenTextures(2, m_pingpongColorBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_pingpongColorBuffer[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, FBOwidth, FBOheight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongColorBuffer[i], 0);

		auto m_pingpongFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (m_pingpongFBOStatus != GL_FRAMEBUFFER_COMPLETE)
			MYTH_ERROR("Framebuffer error: %u", m_pingpongFBOStatus);
	}
}

void ECS::HDR::Update()
{
	Core::System::CameraManager& cam = Core::System::CameraManager::GetInstance();
	if (FBOwidth != cam.WindowWidth || FBOheight != cam.WindowHeight)
	{
		FBOwidth = cam.WindowWidth;
		FBOheight = cam.WindowHeight;

		for (unsigned int i = 0; i < 2; i++)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_colorBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, FBOwidth, FBOheight, 0, GL_RGBA, GL_FLOAT, NULL);
		}

		for (unsigned int i = 0; i < 2; i++)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_pingpongColorBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, FBOwidth, FBOheight, 0, GL_RGBA, GL_FLOAT, NULL);
		}

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FBOwidth, FBOheight);
	}
}

void ECS::HDR::Draw()
{
	bool first_iteration = true;
	m_blurShader->Use();
	glActiveTexture(GL_TEXTURE0);

	for (unsigned int i = 0; i < 10; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);

		m_blurShader->SetInt("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? m_colorBuffer[1] : m_pingpongColorBuffer[!horizontal]);
		renderQuad();
		horizontal = !horizontal;
		first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_hdrShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pingpongColorBuffer[!horizontal]);

	m_hdrShader->SetTextureLocation("uBloomBlur", 1);
	m_hdrShader->SetBool("bloom", m_hdrShader->m_bloom);
	m_hdrShader->SetInt("uHdr", m_hdrShader->m_hdr);
	m_hdrShader->SetFloat("uExposure", m_hdrShader->m_exposure);

	renderQuad();
}

void ECS::HDR::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
