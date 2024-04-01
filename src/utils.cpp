#include "utils.h"

Mesh* ParseObjFile(const char* path, bool withNormals)
{
    std::vector<Vector3> vertices;
    std::vector<Texture> textures;
    std::vector<Vector3> normals;
    std::vector<Triangle> faces;
    std::fstream myfile;

    myfile.open(path, std::ios::in);

    if (!myfile.is_open())
    {
        std::cerr << "Unable to open file\n";
        assert(false);
        return nullptr;
    }

    std::string line;
    while (getline(myfile, line))
    {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        }
        else if (prefix == "vt")
        {
            float u, v;
            ss >> u >> v;
            textures.emplace_back(u, v);
        }
        else if (prefix == "vn" && withNormals)
        {
            float x, y, z;
            ss >> x >> y >> z;
            normals.emplace_back(x, y, z);
        }
        else if (prefix == "f")
        {
            std::vector<int> vIndices, tIndices, nIndices;
            std::string vertex;
            while (ss >> vertex)
            {
                std::replace(vertex.begin(), vertex.end(), '/', ' ');
                std::stringstream vertexStream(vertex);
                int v, t, n;
                vertexStream >> v >> t >> n; // Assuming every vertex in a face has v/t/n format.
                // OBJ indices are 1-based, so we need to subtract 1.
                vIndices.push_back(v-1); 
                tIndices.push_back(t-1);
                nIndices.push_back(n-1);
            }

            // Assuming faces are quads and need to be broken down into two triangles.
            if (vIndices.size() == 4)
            {
                // This is for counter-clockwise winding order.
                // First triangle
                faces.emplace_back(Triangle({vIndices[0], vIndices[1], vIndices[2]},
                                         {tIndices[0], tIndices[1], tIndices[2]},
                                         {nIndices[0], nIndices[1], nIndices[2]}));

                // Second triangle
                faces.emplace_back(Triangle({vIndices[0], vIndices[2], vIndices[3]},
                                         {tIndices[0], tIndices[2], tIndices[3]},
                                         {nIndices[0], nIndices[2], nIndices[3]}));

                // This is for clockwise winding order.
                // // First triangle
                // faces.emplace_back(Face({vIndices[0], vIndices[2], vIndices[1]},
                //                          {tIndices[0], tIndices[2], tIndices[1]},
                //                          {nIndices[0], nIndices[2], nIndices[1]}));

                // // Second triangle
                // faces.emplace_back(Face({vIndices[0], vIndices[3], vIndices[2]},
                //                          {tIndices[0], tIndices[3], tIndices[2]},
                //                          {nIndices[0], nIndices[3], nIndices[2]}));

            }
            else if (vIndices.size() == 3)
            {
                // Directly handle triangles without modification.
                faces.emplace_back(Triangle(vIndices, tIndices, nIndices));
            }
            else
            {
                // Handle other cases or report an error.
                std::cerr << "Unsupported face vertex count: " << vIndices.size() << "\n";
            }
        }
    }

    myfile.close();

    // Basic validation
    if (vertices.empty() || faces.empty())
    {
        std::cerr << "Invalid or empty OBJ file\n";
		assert(false);
		return nullptr;
    }

    // Successful parsing, create and return the mesh
    if (withNormals)
    {
        return new Mesh(vertices, normals, textures, faces);
    }
    else 
    {
        return new Mesh(vertices, textures, faces);
    }
}

ShaderProgram* CreateShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ShaderProgram* shader = new ShaderProgram(vertexShaderPath, fragmentShaderPath);
	return shader;
}

GameObject CreateGameObject(std::string name, std::string objPath, std::string vertexShaderPath, std::string fragmentShaderPath)
{
	Mesh* mesh = ParseObjFile(objPath.c_str(), true);
	ShaderProgram* shader = CreateShaderProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	GameObject gameObject = GameObject(name,mesh, shader);
	return gameObject;
}

GameObject CreateGameObject(std::string name, Mesh* mesh, std::string vertexShaderPath, std::string fragmentShaderPath)
{
	ShaderProgram* shader = CreateShaderProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	GameObject gameObject = GameObject(name, mesh, shader);
	return gameObject;
}

void CheckGLError(const char* file, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error at " << file << ":" << line << " " << err << std::endl;
		assert(false);
	}
}

Quaternion utilsLookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Matrix4 lookMatrix = glm::lookAt(position, target, up);
	Quaternion rotation = glm::quat_cast(lookMatrix);
	return rotation;
}

Quaternion utilsFromAxisAngle(Vector3 axis, double angle)
{
	Quaternion rotation = glm::angleAxis(glm::radians((float)angle), axis);
	return rotation;
}