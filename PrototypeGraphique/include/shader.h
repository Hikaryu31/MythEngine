#pragma once
#include "glad/gl.h"

class Shader
{
public:
	Shader() = default;
	Shader(const char* nameVertex, const char* nameFrag);
	~Shader();

	GLuint Program;

	GLuint CompileShader(GLenum ShaderType, const char* shaderStrs);
	void use();
};