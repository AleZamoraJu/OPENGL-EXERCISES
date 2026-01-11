#include "Model.hpp"

using namespace udit;

Model::Model(const std::string& tex_file_path, const std::string& mesh_file_path): texture(tex_file_path), mesh(mesh_file_path)
{
}

void Model::render(const glm::mat4& model_view, const glm::mat4& projection)
{
    glUseProgram(texture.program_id);

    // Activar textura
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.GetTexId());
    glUniform1i(glGetUniformLocation(texture.program_id, "sampler"), 0);

    // Enviar matrices al shader
    glUniformMatrix4fv(glGetUniformLocation(texture.program_id, "model_view_matrix"), 1, GL_FALSE, glm::value_ptr(model_view));
    glUniformMatrix4fv(glGetUniformLocation(texture.program_id, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection));

    // Renderizar la malla
    mesh.render();
}

