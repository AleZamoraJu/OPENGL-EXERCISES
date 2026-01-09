
// Este código es de dominio público
// angel.rodriguez@udit.es

#include "Cone.hpp"

namespace udit
{

    Cone::Cone()
    {
        float full_circle = 3.14159265f * 2.f;              // un giro completo son 2 * pi radianes (360º)
        unsigned number_of_base_vertices = 10;
        GLfloat cone_radius = 2.4f;

        base_coordinates.push_back(0);
        base_coordinates.push_back(0);
        base_coordinates.push_back(0);

        for (float angle = 0.f, step = full_circle / number_of_base_vertices; angle < full_circle; angle += step)
        {
            float x = cos(angle) * cone_radius;
            float y = 0.f;
            float z = sin(angle) * cone_radius;
            // guardar las coordenadas x, y, z en un vec3 y añadirlo a un vector<vec3> en el que se guardan
            // los vértices del cono


            base_coordinates.push_back(x);
            base_coordinates.push_back(y);
            base_coordinates.push_back(z);
        }
        base_coordinates.push_back(base_coordinates[3]);
        base_coordinates.push_back(base_coordinates[4]);
        base_coordinates.push_back(base_coordinates[5]);

        // Punta del cono

        tip_coordinates.push_back(0);
        tip_coordinates.push_back(5);
        tip_coordinates.push_back(0);

        for (unsigned i = 3; i < base_coordinates.size(); i+=3)
        {
            tip_coordinates.push_back(base_coordinates[i]);
            tip_coordinates.push_back(base_coordinates[i+1]);
            tip_coordinates.push_back(base_coordinates[i+2]);
        }
        tip_coordinates.push_back(base_coordinates[3]);
        tip_coordinates.push_back(base_coordinates[4]);
        tip_coordinates.push_back(base_coordinates[5]);

        normals.push_back(0.f); 
        normals.push_back(-1.f);
        normals.push_back(0.f);
        for (float angle = 0.f, step = full_circle / number_of_base_vertices; angle < full_circle; angle += step) 
        {
            float x = cos(angle);
            float z = sin(angle);
            normals.push_back(x); 
            normals.push_back(0.f);
            normals.push_back(z); 
        } 
        normals.push_back(normals[3]); 
        normals.push_back(normals[4]);
        normals.push_back(normals[5]);

        // Se generan índices para los VBOs del cubo:

        glGenBuffers (2, vbo_ids);
        glGenVertexArrays (1, &vao_id);

        // Se activa el VAO del cubo para configurarlo:

        glBindVertexArray (vao_id);

        // Se suben a un VBO los datos de coordenadas y se vinculan al VAO:

        glBindBuffer (GL_ARRAY_BUFFER, vbo_ids[0]);
        glBufferData (GL_ARRAY_BUFFER, base_coordinates.size()*sizeof(GLfloat), base_coordinates.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Se suben a un VBO los datos de normales y se vinculan al VAO:

        vector <float> base_colors(base_coordinates.size(), 1.f);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[1]);
        glBufferData (GL_ARRAY_BUFFER, base_colors.size()*sizeof(float), base_colors.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Se suben a un IBO los datos de índices:
        glBindVertexArray (0);

        glGenBuffers (2, vbo_ids_tip);
        glGenVertexArrays (1, &vao_id_tip);

        // Se activa el VAO del cubo para configurarlo:

        glBindVertexArray (vao_id_tip);

        // Se suben a un VBO los datos de coordenadas y se vinculan al VAO:

        glBindBuffer (GL_ARRAY_BUFFER, vbo_ids_tip[0]);
        glBufferData (GL_ARRAY_BUFFER, tip_coordinates.size()*sizeof(GLfloat), tip_coordinates.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Se suben a un VBO los datos de normales y se vinculan al VAO:
        vector <float> tip_colors(base_coordinates.size(), 1.f);
        glBindBuffer (GL_ARRAY_BUFFER, vbo_ids_tip[1]);
        glBufferData (GL_ARRAY_BUFFER, tip_colors.size()*sizeof(float), tip_colors.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // Se suben a un IBO los datos de índices:
        glBindVertexArray (0);
    }

    Cone::~Cone()
    {
        // Se liberan los VBOs y el VAO usados:

        glDeleteVertexArrays (1, &vao_id);
        glDeleteBuffers      (2, vbo_ids);
        glDeleteVertexArrays (1, &vao_id_tip);
        glDeleteBuffers      (2, vbo_ids_tip);
    }

    void Cone::render ()
    {
        // Se selecciona el VAO que contiene los datos del objeto y se dibujan sus elementos:
        glFrontFace(GL_CCW);
        glBindVertexArray (vao_id);
        glDrawArrays    (GL_TRIANGLE_FAN, 0, base_coordinates.size() / 3);

        glFrontFace(GL_CW);
        glBindVertexArray (vao_id_tip);
        glDrawArrays    (GL_TRIANGLE_FAN, 0, tip_coordinates.size() / 3);
    }
}
