#include "Resources/Texture.h"
#include <stb_image.h>

REGISTER_RESOURCE_CPP(Resources::Texture)

void Resources::Texture::LoadResource()
{
	if (m_loaded)
		return;

	if (m_embedded)
	{
		int nrChannels;
		Gpu = 0u;
		stbi_set_flip_vertically_on_load(m_flipOnLoad);
		unsigned char* data;

		if (height == 0)
			data = stbi_load_from_memory(texel, width, &width, &height, &nrChannels, 0);
		else
			data = stbi_load_from_memory(texel, width * height * sizeof(texel), &width, &height, &nrChannels, 0);

		if (data)
		{
			glGenTextures(1, &Gpu);

			GLenum format = 0;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, Gpu);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			m_loaded = true;
		}
		else
		{
			MYTH_ERROR("Image loading failed on '%s'\n", Path.c_str());
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}

	else
	{
		int nrChannels;
		Gpu = 0u;
		stbi_set_flip_vertically_on_load(m_flipOnLoad);
		unsigned char* data = stbi_load(Path.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glGenTextures(1, &Gpu);

			GLenum format = 0;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, Gpu);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			m_loaded = true;
		}
		else
		{
			MYTH_ERROR("Image loading failed on '%s'\n", Path.c_str());
		}

		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}
}

void Resources::Texture::UnloadResource()
{
	if (!m_loaded)
		return;

	glDeleteTextures(1, &Gpu);
	Gpu = 0u;
	m_loaded = false;
}

#ifdef MODE_EDITOR
void Resources::Texture::ShowEditorControl()
{
	if (ImGui::Checkbox("Flip texture", &m_flipOnLoad))
	{
		UnloadResource();
		LoadResource();
	}
}
#endif // MODE_EDITOR
