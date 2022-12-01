#include "Resources/Shader.h"
#include <iostream>
#include <fstream>
#include "Core/System/LightManager.h"
#include <fmt/format.h>

REGISTER_RESOURCE_CPP(Resources::Shader)

Resources::Shader::Shader(const std::string& p_name) : Resource(p_name),
	PathVertex("resources/shaders/" + p_name + ".vert"),
	PathFragment("resources/shaders/" + p_name + ".frag"),
	m_program(0u)
{
}

Resources::Shader::Shader(const std::string& p_name, const char* p_vertexShader, const char* p_fragShader) :
	Resource(p_name),
	PathVertex(p_vertexShader),
	PathFragment(p_fragShader),
	m_program(0u)
{
}

Resources::Shader::Shader(const Shader& p_copy) : Resource(p_copy.GetName() + "_copy"), m_program(0u)
{
	PathVertex = p_copy.PathVertex;
	PathFragment = p_copy.PathFragment;
}

Resources::Shader::~Shader()
{
	UnloadResource();
}

GLuint Resources::Shader::CompileShader(GLenum p_ShaderType, const char* p_shaderStrs)
{
	GLuint Shader = glCreateShader(p_ShaderType);

	glShaderSource(Shader, 1, &p_shaderStrs, nullptr);
	glCompileShader(Shader);

	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (CompileStatus == GL_FALSE)
	{
		char Infolog[1024];
		glGetShaderInfoLog(Shader, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
		MYTH_ERROR("Shader error: %s\n", Infolog);
	}

	return Shader;
}

void Resources::Shader::Use()
{
	glUseProgram(m_program);
}

void Resources::Shader::SetVec3(const char* p_name, const Vec3& v)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform3f(m_program, location, v.m_x, v.m_y, v.m_z);
}

void Resources::Shader::SetVec4(const char* p_name, const Vec4& v)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform4f(m_program, location, v.m_x, v.m_y, v.m_z, v.m_w);
}

void Resources::Shader::SetBool(const char* p_name, const bool b)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform1i(m_program, location, b);
}

void Resources::Shader::SetTextureLocation(const char* p_name, const int i)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform1i(m_program, location, i);
}

void Resources::Shader::SetInt(const char* p_name, const int i)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform1i(m_program, location, i);
}

void Resources::Shader::SetFloat(const char* p_name, const float f)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniform1f(m_program, location, f);
}

void Resources::Shader::SetMatrix4x4(const char* p_name, const Mat4x4& m)
{
	GLint location = glGetUniformLocation(m_program, p_name);
	if (location == -1)
		return;

	glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, m.m_mat.data());
}

void Resources::Shader::LoadResource()
{
	if (m_loaded)
		return;

	m_program = glCreateProgram();

	std::ifstream vertexFile(PathVertex);
	std::ifstream fragFile(PathFragment);
	std::string vertexSource;
	std::string fragSource;
	std::string buffer;

	if (vertexFile.is_open())
	{
		while (std::getline(vertexFile, buffer))
			vertexSource += (buffer + '\n');
		vertexFile.close();
	}
	else
		MYTH_WARNING("Unable to open vertex file");

	if (fragFile.is_open())
	{
		while (std::getline(fragFile, buffer))
			fragSource += (buffer + '\n');
		fragFile.close();
	}
	else
		MYTH_WARNING("Unable to open frag file");

	GLuint VertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource.c_str());
	GLuint FragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragSource.c_str());
	GLint success;

	glAttachShader(m_program, VertexShader);
	glAttachShader(m_program, FragmentShader);

	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(m_program, 512, NULL, &infoLog[0]);
		std::string error = "Shader link failed : " + GetName() + "\n" + infoLog;
		MYTH_ERROR(error.c_str());
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	m_loaded = true;
}

void Resources::Shader::UnloadResource()
{
	if (!m_loaded)
		return;

	if (m_program != 0u)
		glDeleteProgram(m_program);

	m_loaded = false;
}

#ifdef MODE_EDITOR
void Resources::Shader::ShowEditorControl()
{
}
#endif // MODE_EDITOR

Resources::HDRShader::HDRShader(const std::string& p_name) : Shader(p_name)
{
}

#ifdef MODE_EDITOR
void Resources::HDRShader::ShowEditorControl()
{
	ImGui::Checkbox("Use HDR", &m_hdr);
	ImGui::Checkbox("Use Bloom", &m_bloom);
	ImGui::DragFloat("Exposure", &m_exposure, 0.1f, 0.f, FLT_MAX);
}
#endif // MODE_EDITOR
