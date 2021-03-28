#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char* path, const std::string& directory);

class Mesh;
class Model
{
public:
	std::vector<Texture> m_textures_loaded;
	std::vector<Mesh> m_meshes;
	std::string m_directory;

	Model(std::string const& path);
	
	void Draw(Shader& shader);

private:
	void LoadModel(std::string const& path);

	void ProcessNode(aiNode* node, const aiScene* scene);

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};