#pragma once
#include "Resources/Resource.h"
#include "glad/glad.h"

#include "Core/Debug/Log.h"

namespace Resources
{
	enum class TYPE_TEXTURE
	{
		NONE,
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		NORMAL_MAP
	};

	class Texture : public Resource
	{
	public:
		Texture(const std::string& p_name, bool nameIsPath = false, bool isEmbedded = false) : Resource(p_name)
		{ if (nameIsPath) Path = p_name; m_embedded = isEmbedded; };
		~Texture() { UnloadResource(); };

		REGISTER_RESOURCE_HPP()

		TYPE_TEXTURE Type = TYPE_TEXTURE::NONE;
		std::string Path = "";
		unsigned char* texel = nullptr;
		GLuint      Gpu  = 0u;
		bool m_flipOnLoad = false;
		bool m_embedded = false;
		int width = 0;
		int height = 0;

		void LoadResource() override;
		void UnloadResource() override;

#ifdef MODE_EDITOR
		void ShowEditorControl() override;
#endif // MODE_EDITOR
	};
}
