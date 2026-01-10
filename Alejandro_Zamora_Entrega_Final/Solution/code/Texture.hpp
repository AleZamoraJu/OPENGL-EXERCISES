#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#pragma once

#include <memory>
#include <string>
#include <SOIL2.h>
#include <glad/gl.h>
#include <Color.hpp>
#include <Color_Buffer.hpp>
#include "opengl-recipes.hpp"

namespace udit
{
    class Texture
    {
    private:
        using Color_Buffer = Color_Buffer<Rgba8888>;

        static const std::string   vertex_shader_code;
        static const std::string fragment_shader_code;

        GLuint texture_id;
        bool   there_is_texture;

        GLint  model_view_matrix_id;
        GLint  projection_matrix_id;

        float  angle;
        float  depth;
        float  speed;

    public:
        GLuint GetTexId();
        GLuint program_id;
        GLuint compile_shaders();
        GLuint create_texture_2d(const std::string& texture_path);

        Texture(const std::string& tex_file_path);
        ~Texture();

    private:
        std::unique_ptr<Color_Buffer> load_image(const std::string& image_path);
    };
}

#endif
