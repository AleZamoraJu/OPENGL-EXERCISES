
#include <string>

#include "Mesh.hpp"
#include "Texture.hpp"

using namespace udit;

class Model
{
private:
	Mesh mesh;
public:
	Texture texture;
	void render(	const glm::mat4& model_view, const glm::mat4& projection);
	Model(const std::string& tex_file_path, const std::string& mesh_file_path);
};