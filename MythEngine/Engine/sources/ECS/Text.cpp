#include "glad/glad.h"
#include "ECS/Text.h"
#include "GLFW/glfw3.h"
#include "imgui_stdlib.h"

#include "Core/System/XMLMacro.h"

REGISTER_COMPONENT_CPP(ECS::Text)

using namespace ECS;

Text::Text() : Text("resources/fonts/OpenSans-Regular.ttf", 50)
{
}

Text::Text(const char* p_path, const unsigned int p_height)
    :m_managerInstance{Core::System::CameraManager::GetInstance()}
{
    m_position = {(float)m_managerInstance.WindowWidth/2.f, (float)m_managerInstance.WindowHeight/2.f };
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, p_path, 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, p_height);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            Vec2((float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows),
            Vec2((float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        m_characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    SetUniforms();

    m_shader.LoadResource();
}

void ECS::Text::Load(tinyxml2::XMLElement* ComponentXml)
{
    tinyxml2::XMLElement* textXML = nullptr;
    if (XMLMacro::GetElement(ComponentXml, &textXML, "text"))
        m_text = textXML->GetText();

    tinyxml2::XMLElement* transformXML = nullptr;
    if (XMLMacro::GetElement(ComponentXml, &transformXML, "transform"))
    {
        m_position.m_x = transformXML->FloatAttribute("position-X", 0.f);
        m_position.m_y = transformXML->FloatAttribute("position-Y", 0.f);
        m_scale = transformXML->FloatAttribute("scale", 1.f);
        m_layer = transformXML->FloatAttribute("layer", 0.f);
    }

    tinyxml2::XMLElement* colorXML = nullptr;
    if (XMLMacro::GetElement(ComponentXml, &colorXML, "color"))
    {
        m_color.m_x = colorXML->FloatAttribute("R", 1.f);
        m_color.m_y = colorXML->FloatAttribute("G", 1.f);
        m_color.m_z = colorXML->FloatAttribute("B", 1.f);
    }
}

#ifdef MODE_EDITOR
void Text::ShowEditorControl(const unsigned int p_indexComponent)
{
    ImGui::Spacing();

    ImGui::InputText(("##Text-" + std::to_string(p_indexComponent)).c_str(), &m_text);
    float* color[3] = { &m_color.m_x , &m_color.m_y, &m_color.m_z };
    ImGui::DragFloat2(("position##Text-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_position, 0.1f);
    if( ImGui::DragFloat(("scale##Text-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_scale, 0.01f) &&
        m_scale < 0.1f)
        m_scale = 0.1f;
    ImGui::DragFloat(("layer##Text-" + std::to_string(p_indexComponent)).c_str(), (float*)&m_layer, 0.25f);
    ImGui::ColorEdit3(("color##Text-" + std::to_string(p_indexComponent)).c_str(), *color);
    // multiply triangle's color with this color
    m_shader.SetVec3("textColor", m_color);
}

void ECS::Text::Save(tinyxml2::XMLElement* ComponentXml)
{
    tinyxml2::XMLElement* textXML = ComponentXml->InsertNewChildElement("text");
    textXML->SetText(m_text.c_str());

    tinyxml2::XMLElement* transformXML = ComponentXml->InsertNewChildElement("transform");
    transformXML->SetAttribute("position-X", m_position.m_x);
    transformXML->SetAttribute("position-Y", m_position.m_y);
    transformXML->SetAttribute("scale", m_scale);
    transformXML->SetAttribute("layer", m_layer);

    tinyxml2::XMLElement* colorXML = ComponentXml->InsertNewChildElement("color");
    colorXML->SetAttribute("R", m_color.m_x);
    colorXML->SetAttribute("G", m_color.m_y);
    colorXML->SetAttribute("B", m_color.m_z);
}
#endif // MODE_EDITOR

void Text::DrawUi(Core::GameObject* p_gameObject)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float x = m_position.m_x;
    float y = m_position.m_y;
    // activate corresponding render state	
    m_shader.Use();

    m_shader.SetMatrix4x4("projection", m_projection);
    m_shader.SetVec3("textColor", m_color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = m_text.begin(); c != m_text.end(); c++)
    {
        Character ch = m_characters[*c];

        float xpos = x + ch.m_bearing.m_x * m_scale;
        float ypos = y - (ch.m_size.m_y - ch.m_bearing.m_y) * m_scale;

        float w = ch.m_size.m_x * m_scale;
        float h = ch.m_size.m_y * m_scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.m_textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.m_advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void Text::UpdateWindow()
{
    if ((float)m_managerInstance.WindowWidth != m_frameSize.m_x ||
        (float)m_managerInstance.WindowHeight != m_frameSize.m_y)
    {
        m_frameSize = { (float)m_managerInstance.WindowWidth, (float)m_managerInstance.WindowHeight };

        m_projection = Mat4x4::OrthoScreenToGL(m_frameSize.m_x, m_frameSize.m_y);
    }
}

void Text::SetUniforms()
{
    m_frameSize = { (float)m_managerInstance.WindowWidth, (float)m_managerInstance.WindowHeight };

    m_projection = Mat4x4::OrthoScreenToGL(m_frameSize.m_x, m_frameSize.m_y);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}