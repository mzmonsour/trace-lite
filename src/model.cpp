#include "model.h"

Model::Model(  const std::string name,
                std::vector<glm::vec4> vertices,
                std::vector<glm::vec4> normals,
                std::vector<FaceIndex> triangles) :
    m_name(name),
    m_vertices(std::move(vertices)),
    m_normals(std::move(normals)),
    m_triangles(std::move(triangles))
{
}
