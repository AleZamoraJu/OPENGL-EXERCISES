#include "Mesh.hpp"

Mesh::Mesh(const std::string& path)
{
    load_mesh(path);
}

void Mesh::load_mesh(const std::string& mesh_file_path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile( mesh_file_path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    if (!scene || scene->mNumMeshes == 0)
    {
        std::cerr << "Error loading mesh: " << mesh_file_path << std::endl;
        return;
    }

    // Limpiar submeshes anteriores si existían
    submeshes.clear();

    // Recorrer todas las mallas del modelo
    for (unsigned m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];
        const size_t vertex_count = mesh->mNumVertices;

        // 1️ Copiar posiciones
        std::vector<glm::vec3> positions(vertex_count);
        for (size_t i = 0; i < vertex_count; ++i)
        {
            positions[i] = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
        }

        SubMesh sm;

        // 2️ Colores por defecto (gris)
        std::vector<glm::vec2> uvs(vertex_count, glm::vec2(0.f));
        if (mesh->mTextureCoords[0])
        {
            for (size_t i = 0; i < vertex_count; ++i)
                uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        
        if (!mesh->mTextureCoords[0])
        {
            std::cout << "¡Advertencia! UVs no encontradas en mesh " << mesh_file_path << std::endl;
        }


        // VBO de UVs
        glBindBuffer(GL_ARRAY_BUFFER, sm.vbo[COLORS_VBO]); // renombrar a TEXCOORDS_VBO
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);


        // 3️ Índices
        GLsizei index_count = mesh->mNumFaces * 3;
        std::vector<GLuint> indices;
        indices.reserve(index_count);

        for (unsigned i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            assert(face.mNumIndices == 3);

            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        // 4️ Crear VAO y VBOs
        sm.index_count = index_count;
        glGenVertexArrays(1, &sm.vao);
        glBindVertexArray(sm.vao);

        glGenBuffers(VBO_COUNT, sm.vbo);

        // Posiciones
        glBindBuffer(GL_ARRAY_BUFFER, sm.vbo[COORDINATES_VBO]);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // Colores
        glBindBuffer(GL_ARRAY_BUFFER, sm.vbo[COLORS_VBO]); // renombrar
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);


        // Índices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sm.vbo[INDICES_EBO]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Limpiar VAO
        glBindVertexArray(0);

        // Guardar submesh
        submeshes.push_back(sm);
    }

    std::cout << "Loaded " << submeshes.size() << " submeshes from " << mesh_file_path << std::endl;
}

void Mesh::render()
{
    // Renderizar todos los submeshes
    for (const auto& sm : submeshes)
    {
        glBindVertexArray(sm.vao);
        glDrawElements(GL_TRIANGLES, sm.index_count, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(2, vbo_ids);
}