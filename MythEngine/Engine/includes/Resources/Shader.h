#pragma once
#include "Resources/Resource.h"
#include "glad/glad.h"
#include "maths/Mat4x4.hpp"

#include "Core/Debug/Log.h"

namespace Resources
{
	class Shader : public Resource
	{
	public :
		Shader(const std::string& p_name);
		Shader(const std::string& name, const char* p_vertexShader, const char* p_fragShader);
		Shader(const Shader& p_copy);
		~Shader();

		REGISTER_RESOURCE_HPP()

		std::string PathVertex;
		std::string PathFragment;

		GLuint CompileShader(GLenum p_ShaderType, const char* p_shaderStrs);
		void Use();

		void SetVec3(const char* p_name, const Vec3& v);
		void SetVec4(const char* p_name, const Vec4& v);
		void SetBool(const char* p_name, const bool b);
		void SetTextureLocation(const char* p_name, const int i);
		void SetInt(const char* p_name, const int i);
		void SetFloat(const char* p_name, const float f);
		void SetMatrix4x4(const char* p_name, const Mat4x4& m);

		//GLint GetUniform(const char* p_name);

		void LoadResource() override;
		void UnloadResource() override;

#ifdef MODE_EDITOR
		 void ShowEditorControl() override;
#endif // MODE_EDITOR

	protected:
		GLuint m_program;
	};

	class HDRShader : public Shader
	{
	public:
		HDRShader(const std::string& p_name);

		bool m_hdr = true;
		bool m_bloom = true;
		float m_exposure = 1.f;

#ifdef MODE_EDITOR
		void ShowEditorControl() override;
#endif // MODE_EDITOR
	};
}