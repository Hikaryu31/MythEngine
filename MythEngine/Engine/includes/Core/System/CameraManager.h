#pragma once

#include "define.h"

#include "Core/BaseSingleton.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ECS/Camera.h"

namespace Core
{
	namespace System
	{
		class CameraManager : public BaseSingleton<CameraManager>
		{
		public:
			CameraManager();
			~CameraManager() { singleton = nullptr; }

			static void ResizeCallback(GLFWwindow* window, int width, int height);
			void Setup(GLFWwindow* window);

			ECS::Camera* GetCamera() { return m_currentCamera; }
			bool HasCamera() { return (m_currentCamera != nullptr); }
			bool CameraIsOrthographic() { return (m_currentCamera != nullptr ? m_currentCamera->IsOrthographic() : false); }
			void SetCamera(ECS::Camera* p_newCamera, const bool p_setSelected = true);
			
			int WindowHeight;
			int WindowWidth;

			float Aspect;
			float FovY;
			float Near;
			float Far;

			static Mat4x4 GetProjectionMatrix();

		private:
			ECS::Camera* m_currentCamera;
		};
	}
}