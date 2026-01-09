#pragma once

#include<string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/gl.h>
#include <glm.hpp>							// vec3, vec4, ivec4, mat4
	
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>                 // value_ptr

#include <vector>

#include <iostream>

using std::vector;
using glm::vec3;

using namespace glm;

class Mesh
{
    private:
        enum
        {
            COORDINATES_VBO,
            COLORS_VBO,
            INDICES_EBO,
            VBO_COUNT
        };

        GLuint  vbo_ids[VBO_COUNT];
        GLuint  vao_id;

        GLsizei number_of_indices;

        struct SubMesh
        {
            GLuint vao;
            GLuint vbo[3];
            GLsizei index_count;
        };

        std::vector<SubMesh> submeshes;

    public:
    	Mesh(const std::string& path);
    	void   load_mesh(const std::string& mesh_file_path);
        void   render();
        ~Mesh();
};