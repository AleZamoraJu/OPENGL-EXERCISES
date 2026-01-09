
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef CUBE_HEADER
#define CUBE_HEADER

    #include <glad/gl.h>
    #include <vector>;
    using std::vector;

    namespace udit
    {

        class Cone
        {
        private:

            // Arrays de datos del cubo base:

            vector <GLfloat> base_coordinates;
            vector <GLfloat> tip_coordinates;
            vector <GLfloat> normals;
            static const GLubyte indices    [];

        private:

            GLuint vbo_ids[2];      // Ids de los VBOs que se usan
            GLuint vao_id;          // Id del VAO del cubo
            GLuint vbo_ids_tip[2];     
            GLuint vao_id_tip;                  

        public:

            Cone();
           ~Cone();

            void render ();
            void renderWireframe();
        };

    }

#endif
