#include "model.h"
#include <glm/vec3.hpp>

Model::Model(  const std::string name,
                std::vector<glm::vec3> vertices,
                std::vector<glm::vec3> normals,
                std::vector<FaceIndex> triangles) :
    m_name(name),
    m_vertices(std::move(vertices)),
    m_normals(std::move(normals)),
    m_triangles(std::move(triangles))
{
}
