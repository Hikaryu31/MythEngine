#include "shader.h"
#include <vector>

Shader::Shader(const char* nameVertex, const char* nameFrag)
{
	Program = glCreateProgram();

	GLuint VertexShader = CompileShader(GL_VERTEX_SHADER, nameVertex);
	GLuint FragmentShader = CompileShader(GL_FRAGMENT_SHADER, nameFrag);

	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);

	glLinkProgram(Program);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(Program);
}

GLuint Shader::CompileShader(GLenum ShaderType, const char* shaderStrs)
{
	GLuint Shader = glCreateShader(ShaderType);

	glShaderSource(Shader, 1, &shaderStrs, nullptr);
	glCompileShader(Shader);

	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (CompileStatus == GL_FALSE)
	{
		char Infolog[1024];
		glGetShaderInfoLog(Shader, sizeof(Infolog) / sizeof(Infolog[0]), nullptr, Infolog);
		fprintf(stderr, "Shader error: %s\n", Infolog);
	}

	return Shader;
}

void Shader::use()
{
	glUseProgram(Program);
}