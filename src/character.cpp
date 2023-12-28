#include "character.h"

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "material.h"
#include "model.h"
#include "shader.h"
#include "globals.h"
#include "camera.h"

Character_Render::Character_Render(const char* font)
{
    init_fonts(font);
    m_projection = glm::ortho<float>(0.f, util::Globals::camera.width(), 0.f, util::Globals::camera.height());
}

void Character_Render::render_text(const Shader::Text_shader& shader, std::string texts, float x, float y, 
    float scale, const glm::vec3& color) const
{
    assert(shader.shader_name() == "./shader/text_shader"); 
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    shader.use();
    for(std::string::const_iterator it = texts.begin(); it != texts.end(); ++it)
    {
        Character ch = m_characters.at(*it);

        float pos_x = x + ch.bearing.x * scale;
        float pos_y = y + (ch.bearing.y - ch.size.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float data[24] = {
            pos_x, pos_y,         0.f, 0.f,
            pos_x + w, pos_y,     1.f, 0.f,
            pos_x, pos_y + h,     0.f, 1.f,

            pos_x, pos_y + h,     0.f, 1.f,
            pos_x + w, pos_y,     1.f, 0.f,
            pos_x + w, pos_y + h, 1.f, 1.f
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, data);
        glEnableVertexAttribArray(0);
        shader.set_uniforms(m_projection, color, ch.texture->texunit);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.advance >> 6) * scale;
    }
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Character_Render::init_fonts(const char* font)
{
    FT_Library ft;
	if(FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library.\n";
		exit(1);
	}
	FT_Face face;
	if(FT_New_Face(ft, font, 0, &face))
	{
		std::cout << "ERROR:FREETYPE: Could not load font from './Fonts/arial.ttf'.\n";
		exit(1);
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
    
    // Load characters to unordered_map
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(unsigned char c = 0; c < 128; ++c)
    {
        Character character;
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Could not load character " << c << ".\n";
            exit(1);
        }
        Texture* character_tex = new Texture();
        glGenTextures(1, &character_tex->texbuffer);
        character_tex->texname = c;
        character_tex->texunit = Model::fatch_new_texunit();
        glActiveTexture(GL_TEXTURE0 + character_tex->texunit);
        glBindTexture(GL_TEXTURE_2D, character_tex->texbuffer);
        Model::add_texture(character_tex->texname.c_str(), *character_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        character.texture = new Texture(*character_tex);
        character.size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        character.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
        character.advance = face->glyph->advance.x;
        
        m_characters.insert(std::pair<char, Character>(c, character));
    }
    
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // init VAO, VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}