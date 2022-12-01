#include "Core/Debug/Draw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "maths/Mat4x4.hpp"
#include "Core/System/CameraManager.h"

SINGLETON_SETUP(Core::Debug::Draw)

void Core::Debug::Draw::Line(const Vec3& p_start, const Vec3& p_end, const Vec3& p_color)
{
#ifdef MODE_EDITOR
	MYTH_ASSERT(singleton != nullptr);

	singleton->m_lineList.emplace_back(p_start, p_color, p_end, p_color);
#endif // MODE_EDITOR
}

void Core::Debug::Draw::Render()
{
	//GLfloat points[12];

	if (m_shaderLine == 0u)
	{
		GLint Result = GL_FALSE;
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		//int InfoLogLength = 1024;

		// Compile Vertex Shader
		char const* VertexSourcePointer = R"GLSL(
#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 uVP;

out vec3 fColor;

void main()
{
	gl_Position = uVP * vec4(position, 1.0);
	fColor = color;
}
		)GLSL";

		glShaderSource(vertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(vertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(vertexShaderID, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("VShader error: %s\n", Infolog);
		}

		// Compile Fragment Shader
		char const* FragmentSourcePointer = R"GLSL(
#version 450 core

in vec3 fColor;

out vec4 color;

void main()
{
    color = vec4(fColor, 1.0);   
}
		)GLSL";
		glShaderSource(fragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(fragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(fragmentShaderID, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("FShader error: %s\n", Infolog);
		}

		// Link the program
		m_shaderLine = glCreateProgram();
		glAttachShader(m_shaderLine, vertexShaderID);
		glAttachShader(m_shaderLine, fragmentShaderID);
		glLinkProgram(m_shaderLine);

		// Check Fragment Shader
		glGetShaderiv(m_shaderLine, GL_COMPILE_STATUS, &Result);
		//glGetShaderiv(m_shaderLine, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE)
		{
			char Infolog[1024];
			glGetShaderInfoLog(m_shaderLine, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
			MYTH_ERROR("Program Shader error: %s\n", Infolog);
		}

		glDetachShader(m_shaderLine, vertexShaderID);
		glDetachShader(m_shaderLine, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}

	glUseProgram(m_shaderLine);

	Core::System::CameraManager& CM = Core::System::CameraManager::GetInstance();

	Mat4x4 ViewProj = CM.GetProjectionMatrix();
	if (CM.HasCamera())
		ViewProj = ViewProj * CM.GetCamera()->GetViewMatrix();

	glProgramUniformMatrix4fv(m_shaderLine, glGetUniformLocation(m_shaderLine, "uVP"), 1, GL_FALSE, ViewProj.m_mat.data());

	GLuint VBO = 0u;
	GLuint VAO = 0u;

	//for (sDrawLineData& DLdata : m_lineList)
	//{
		//MYTH_DEBUG("Draw::Line call %u", m_lineList.size());

		/*points[0] = DLdata.Start.m_x;
		points[1] = DLdata.Start.m_y;
		points[2] = DLdata.Start.m_z;
		points[3] = DLdata.Color.m_x;
		points[4] = DLdata.Color.m_y;
		points[5] = DLdata.Color.m_z;

		points[6] = DLdata.End.m_x;
		points[7] = DLdata.End.m_y;
		points[8] = DLdata.End.m_z;
		points[9] = DLdata.Color.m_x;
		points[10] = DLdata.Color.m_y;
		points[11] = DLdata.Color.m_z;/**/

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, m_lineList.size() * sizeof(sDrawLineData), m_lineList.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glDrawArrays(GL_LINES, 0, m_lineList.size() * 2);

		glBindVertexArray(0u);
		glBindBuffer(GL_ARRAY_BUFFER, 0u);
	//}

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	m_lineList.clear();
}
