
// Este código es de dominio público
// angel.rodriguez@udit.es

#include "Scene.hpp"

#include <glm.hpp>                          // vec3, vec4, mat4, etc.
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>                 // value_ptr

#include <opengl-recipes.hpp>

namespace udit
{

    using namespace std;

    const string Scene::vertex_shader_code =

        "#version 330\n"
        ""
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        ""
        "layout (location = 0) in vec3 vertex_xz;"
        "layout (location = 1) in vec2 vertex_uv;"
        ""
        "uniform sampler2D sampler;"
        "uniform float     max_height;"
        "uniform float line_color;"
        "out float intensity;"
        ""
        "void main()"
        "{"
        "   float sample = texture (sampler, vertex_uv).r;"
        "   intensity    = line_color * (sample * 0.75 + 0.25);"
        "   float height = sample * max_height;"
        "   vec4  xyzw   = vec4(vertex_xz.x, height, vertex_xz.y, 1.0);"
        "   gl_Position  = projection_matrix * model_view_matrix * xyzw;"
        "}";

    const string Scene::fragment_shader_code =

        "#version 330\n"
        "in  float intensity;"
        "out vec4  fragment_color;"
        ""
        "void main()"
        "{"
        "    fragment_color = vec4(intensity, intensity, intensity, 1);"
        "}";

    const string Scene::vertex_shader_cone_code =

        "#version 330\n"
        ""
        "struct Light"
        "{"
        "    vec4 position;"
        "    vec3 color;"
        "};"
        ""
        "uniform Light light;"
        "uniform float ambient_intensity;"
        "uniform float diffuse_intensity;"
        "uniform float opacity;"
        "out float frag_opacity;"
        ""
        "uniform vec3 material_color;"
        ""
        "uniform mat4 model_view_matrix;"
        "uniform mat4 projection_matrix;"
        "uniform mat4     normal_matrix;"
        ""
        "layout (location = 0) in vec3 vertex_coordinates;"
        "layout (location = 1) in vec3 vertex_normal;"
        ""
        "out vec3 front_color;"
        ""
        "void main()"
        "{"
        "    vec4  normal   = normal_matrix * vec4(vertex_normal, 0.0);"
        "    vec4  position = model_view_matrix * vec4(vertex_coordinates, 1.0);"
        ""
        "    vec4  light_direction = light.position - position;"
        "    float light_intensity = diffuse_intensity * max (dot (normalize (normal.xyz), normalize (light_direction.xyz)), 0.0);"
        ""
        "    front_color = ambient_intensity * material_color + diffuse_intensity * light_intensity * light.color * material_color;"
        "    gl_Position = projection_matrix * position;"
        ""
        "    frag_opacity = opacity;"
        "}";

    const string Scene::fragment_shader_cone_code =

        "#version 330\n"
        ""
        "in float  frag_opacity;"
        "in  vec3    front_color;"
        "out vec4 fragment_color;"
        ""
        "void main()"
        "{"
        "    fragment_color = vec4(front_color, frag_opacity);"
        "}";

    const string Scene::texture_path = "../../../shared/assets/height-map.png";

    Scene::Scene(int width, int height)
    :
        terrain(10.f, 10.f, 50, 50),
        angle  (0.f),
        cone()
    {
        // Se compilan y se activan los shaders:

        program_id = compile_shaders (vertex_shader_code, fragment_shader_code);

        glUseProgram (program_id);

        program_id_2 = compile_shaders (vertex_shader_cone_code, fragment_shader_cone_code);

        glUseProgram (program_id_2);

        model_view_matrix_id = glGetUniformLocation (program_id, "model_view_matrix");
        model_view_matrix_id_2 = glGetUniformLocation (program_id_2, "model_view_matrix");

        projection_matrix_id = glGetUniformLocation (program_id, "projection_matrix");

        // Se establece la altura máxima del height map en el vertex shader:

        glUniform1f (glGetUniformLocation (program_id, "max_height"), 5.f);

        // Se carga la textura y se envía a la GPU:

        texture_id = create_texture_2d< Monochrome8 > (texture_path);

        there_is_texture = texture_id > 0;

        // Se establece la configuración básica:

        glEnable     (GL_CULL_FACE );
        glEnable     (GL_DEPTH_TEST);
        glClearColor (0.1f, 0.1f, 0.1f, 1.f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        resize (width, height);
    }

    Scene::~Scene()
    {
        glDeleteProgram (program_id);

        if (there_is_texture)
        {
            glDeleteTextures (1, &texture_id);
        }
    }

    void Scene::update ()
    {
        angle += .005f;
    }

    void Scene::render()
    {
        // 1️⃣ Limpiar buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2️⃣ Matrices de cámara y modelo
        glm::mat4 model_view_matrix(1.f);
        model_view_matrix = glm::translate(model_view_matrix, glm::vec3(0.f, -5.f, -20.f)); // ajustar altura y distancia
        model_view_matrix = glm::rotate(model_view_matrix, angle, glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 projection_matrix = glm::perspective(
            glm::radians(45.f),         // FOV vertical
            1.f, // aspect ratio
            10.f,                       // near plane
            500.f                        // far plane
        );

        glm::mat4 normal_matrix = glm::transpose(glm::inverse(model_view_matrix));

        // ========================
        // 3️⃣ Render terreno (shader 1)
        // ========================
        glUseProgram(program_id);

        // Enviar matrices
        glUniformMatrix4fv(model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Activar textura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(program_id, "sampler"), 0);

        // Altura máxima
        glUniform1f(glGetUniformLocation(program_id, "max_height"), 5.f);

        // Color
        glUniform1f(glGetUniformLocation(program_id, "line_color"), 1.0f);

        // Render terreno
        terrain.render();

        // Color
        glUniform1f(glGetUniformLocation(program_id, "line_color"), 0.f);

        // Render terreno
        terrain.renderWireframe();



        // ========================
        // 4️⃣ Render cono (shader 2)
        // ========================
        glUseProgram(program_id_2);

        glUniformMatrix4fv(model_view_matrix_id_2, 1, GL_FALSE, glm::value_ptr(model_view_matrix));
        glUniformMatrix4fv(glGetUniformLocation(program_id_2, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));
        glUniformMatrix4fv(glGetUniformLocation(program_id_2, "normal_matrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));

        // Parámetros de luz y material
        glUniform3f(glGetUniformLocation(program_id_2, "material_color"), 1.f, 1.f, 1.f);
        glUniform4f(glGetUniformLocation(program_id_2, "light.position"), 10.f, 10.f, 10.f, 1.f);
        glUniform3f(glGetUniformLocation(program_id_2, "light.color"), 1.f, 1.f, 1.f);
        glUniform1f(glGetUniformLocation(program_id_2, "ambient_intensity"), 0.3f);
        glUniform1f(glGetUniformLocation(program_id_2, "diffuse_intensity"), 0.7f);

        //Opacidad
        glUniform1f(glGetUniformLocation(program_id_2, "opacity"), 0.2f);

        glm::mat4 cone_model_matrix = model_view_matrix;

        // rotación fija (una sola vez)
        cone_model_matrix = glm::rotate(cone_model_matrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

        // rotación (cada frame)
        cone_model_matrix = glm::rotate(cone_model_matrix, -angle, glm::vec3(0.f, 0.f, 1.f));

        // traslación
        cone_model_matrix = glm::translate(cone_model_matrix, glm::vec3(0.f, -5.f, -5.f));

        // enviar al shader
        glUniformMatrix4fv(model_view_matrix_id_2, 1, GL_FALSE, glm::value_ptr(cone_model_matrix));

        // asignamos a la normal matrix la inversa de la matriz del cono para la ilumminacion
        glUniformMatrix4fv(glGetUniformLocation(program_id_2, "normal_matrix"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(cone_model_matrix))));

        // render
        cone.render();

    }

    void Scene::resize (int width, int height)
    {
        glm::mat4 projection_matrix = glm::perspective (20.f, GLfloat(width) / height, 1.f, 500.f);

        glUniformMatrix4fv (projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        glViewport (0, 0, width, height);
    }

}
