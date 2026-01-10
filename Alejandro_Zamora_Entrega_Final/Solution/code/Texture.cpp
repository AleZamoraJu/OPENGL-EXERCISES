#include"Texture.hpp"

using namespace udit;

const std::string Texture::vertex_shader_code =

"#version 330\n"
""
"uniform mat4 model_view_matrix;"
"uniform mat4 projection_matrix;"
""
"layout (location = 0) in vec3 vertex_coordinates;"
"layout (location = 1) in vec2 vertex_texture_uv;"
""
"out vec2 texture_uv;"
""
"void main()"
"{"
"   gl_Position = projection_matrix * model_view_matrix * vec4(vertex_coordinates, 1.0);"
"   texture_uv  = vertex_texture_uv;"
"}";

const std::string Texture::fragment_shader_code =

"#version 330\n"
""
"uniform sampler2D sampler2d;"
""
"in  vec2 texture_uv;"
"out vec4 fragment_color;"
""
"void main()"
"{"
"    fragment_color = vec4(texture (sampler2d, texture_uv.st).rgb, 1.0);"
"}";

Texture::Texture(const std::string& tex_file_path) : angle(0.0f), depth(-5.0f), speed(-0.2f)
{
    program_id = compile_shaders();

    glUseProgram(program_id);

    model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
    projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");

    // Se carga la textura y se envía a la GPU:

    texture_id = create_texture_2d(tex_file_path);

    there_is_texture = texture_id > 0;

    // Se establece la configuración básica:

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
}

GLuint Texture::create_texture_2d(const std::string& texture_path)
{
    auto image = load_image(texture_path);

    if (image)
    {
        // Se habilitan las texturas, se genera un id para un búfer de textura,
        // se selecciona el búfer de textura creado y se configuran algunos de
        // sus parámetros:

        GLuint texture_id;

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image->get_width(), image->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->colors() );

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        return texture_id;
    }

    return -1;
}

std::unique_ptr< Texture::Color_Buffer > Texture::load_image(const std::string& image_path)
{
    // Se carga la imagen del archivo usando SOIL2:

    int image_width = 0;
    int image_height = 0;
    int image_channels = 0;

    uint8_t* loaded_pixels = SOIL_load_image
    (
        image_path.c_str(),
        &image_width,
        &image_height,
        &image_channels,
        SOIL_LOAD_RGBA              // Indica que nos devuelva los pixels en formato RGB32
    );                              // al margen del formato usado en el archivo

    // Si loaded_pixels no es nullptr, la imagen se ha podido cargar correctamente:

    if (loaded_pixels)
    {
        auto image = std::make_unique< Color_Buffer >(image_width, image_height);

        // Se copian los bytes de un buffer a otro directamente:

        std::copy_n
        (
            loaded_pixels,
            size_t(image_width) * size_t(image_height) * sizeof(Color_Buffer::Color),
            reinterpret_cast<uint8_t*>(image->colors())
        );

        // Se libera la memoria que reservó SOIL2 para cargar la imagen:

        SOIL_free_image_data(loaded_pixels);

        return image;
    }

    return nullptr;
}

GLuint Texture::compile_shaders()
{
    GLint succeeded = GL_FALSE;

    // Se crean objetos para los shaders:

    GLuint   vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Se carga el código de los shaders:

    const char* vertex_shaders_code[] = { vertex_shader_code.c_str() };
    const char* fragment_shaders_code[] = { fragment_shader_code.c_str() };
    const GLint    vertex_shaders_size[] = { (GLint)vertex_shader_code.size() };
    const GLint  fragment_shaders_size[] = { (GLint)fragment_shader_code.size() };

    glShaderSource(vertex_shader_id, 1, vertex_shaders_code, vertex_shaders_size);
    glShaderSource(fragment_shader_id, 1, fragment_shaders_code, fragment_shaders_size);

    // Se compilan los shaders:

    glCompileShader(vertex_shader_id);
    glCompileShader(fragment_shader_id);

    // Se comprueba que si la compilación ha tenido éxito:

    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &succeeded);
    if (!succeeded) show_compilation_error(vertex_shader_id);

    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &succeeded);
    if (!succeeded) show_compilation_error(fragment_shader_id);

    // Se crea un objeto para un programa:

    GLuint program_id = glCreateProgram();

    // Se cargan los shaders compilados en el programa:

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Se linkan los shaders:

    glLinkProgram(program_id);

    // Se comprueba si el linkage ha tenido éxito:

    glGetProgramiv(program_id, GL_LINK_STATUS, &succeeded);
    if (!succeeded) show_linkage_error(program_id);

    // Se liberan los shaders compilados una vez se han linkado:

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}

GLuint Texture::GetTexId()
{
    return texture_id;
}

Texture::~Texture()
{
    if (there_is_texture)
        glDeleteTextures(1, &texture_id);

    glDeleteProgram(program_id);
}

