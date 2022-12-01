#ifndef _TEXT_MANAGER_HPP_
#define _TEXT_MANAGER_HPP_

#include <map>
#include <iostream>

#include "Maths/Mat4x4.hpp"
#include "Resources/Shader.h"
#include "ECS/Component.h"
#include "Core/System/CameraManager.h"

#include "ft2build.h"
#include FT_FREETYPE_H

struct Character
{
	unsigned int m_textureID;  // ID handle of the glyph texture
	Vec2		 m_size;       // Size of glyph
	Vec2		 m_bearing;    // Offset from baseline to left/top of glyph
	unsigned int m_advance;    // Offset to advance to next glyph
};

namespace ECS
{
	class Text : public Component, public UiRenderComponent, public GameplayComponent
	{
	public:
		Text();
		Text(const char* p_path , const unsigned int p_height = 50);
		~Text() = default;

		REGISTER_COMPONENT_HPP(COMPONENT_TYPE_UI_RENDER | COMPONENT_TYPE_GAMEPLAY)

		std::string		m_text = "Sample text";
		
		Vec2	m_position;
		float	m_scale = 1;
		Vec3	m_color = Vec3::One;

		Resources::Shader	m_shader = Resources::Shader("TextShader","resources/shaders/basicText.vert","resources/shaders/basicText.frag");

		unsigned int m_VAO;
		unsigned int m_VBO;

		Core::System::CameraManager& m_managerInstance;

		void Start(Core::GameObject* p_gameObject) override { SetUniforms(); };
		void Update(Core::GameObject* p_gameObject) override { UpdateWindow(); };
		void Load(tinyxml2::XMLElement* ComponentXml) override;
#ifdef MODE_EDITOR
		void EditorUpdate(Core::GameObject* p_gameObject) override { UpdateWindow(); }
		void ShowEditorControl(const unsigned int p_indexComponent) override;
		void Save(tinyxml2::XMLElement* ComponentXml) override;
#endif // MODE_EDITOR
		void DrawUi(Core::GameObject* p_gameObject) override;

		float GetLayer() const override { return m_layer; }

		void UpdateWindow();
		void SetUniforms();

	private:

		bool	m_enabled = true;
		bool	m_deleteFlag;

		std::map<char, Character>	m_characters;

		Mat4x4	m_projection;
		Vec2	m_frameSize;
		float   m_layer = 0.f;
	};
}


#endif // !_TEXT_MANAGER_HPP_
