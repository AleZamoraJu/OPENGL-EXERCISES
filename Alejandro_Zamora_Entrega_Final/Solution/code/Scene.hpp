
// Este código es de dominio público
// angel.rodriguez@udit.es

#ifndef VIEW_HEADER
#define VIEW_HEADER

    #include <Color.hpp>
    #include <Color_Buffer.hpp>
    #include <glad/gl.h>
    #include <string>
    #include "Terrain.hpp"
    #include "Cone.hpp"
    #include "Model.hpp"

    namespace udit
    {

        class Scene
        {
        private:

            typedef Color_Buffer< Monochrome8 > Color_Buffer;

        private:

            static const  std::string   vertex_shader_code;
            static const  std::string   fragment_shader_code;
            static const  std::string   vertex_shader_cone_code;
            static const  std::string   fragment_shader_cone_code;
            static const  std::string   texture_uvs;
            static const  std::string   texture_path;
            static const  std::string   model_path;

            GLuint  program_id;
            GLuint  program_id_2;

            GLuint  texture_id;
            bool    there_is_texture;

            GLint   model_view_matrix_id;
            GLint   model_view_matrix_id_2;

            GLint   projection_matrix_id;

            Terrain terrain;
            Cone    cone; 
            Model    lighthouse;

            float   angle;

        public:

            Scene(int width, int height);
           ~Scene();

            void update ();
            void render ();
            void resize (int  width, int height);

        };

    }

#endif
