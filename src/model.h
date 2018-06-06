#pragma once

#include <glm/fwd.hpp>
#include <vector>
#include <string>

struct FaceIndex {
    int vertices[3];
    int normals[3];
};

class Model {
    private:

        const std::string m_name;

        std::vector<glm::vec3>  m_vertices;
        std::vector<glm::vec3>  m_normals;
        std::vector<FaceIndex>  m_triangles;

    public:

        /**
         * Create a model from a list of vertices, normals, and triangles.
         */
        Model(  const std::string name,
                std::vector<glm::vec3> vertices,
                std::vector<glm::vec3> normals,
                std::vector<FaceIndex> triangles);

        Model() {}

        ~Model() {}

        /**
         * Get the name of the model.
         */
        const std::string& get_name() const { return m_name; }

        /**
         * Get the model's vertices.
         */
        const std::vector<glm::vec3>& get_vertices() const { return m_vertices; }
};
