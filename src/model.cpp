#include "model.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

Model::Model(  const std::string name,
                std::vector<vec4> vertices,
                std::vector<vec4> normals,
                std::vector<FaceIndex> triangles) :
    m_name(name),
    m_vertices(std::move(vertices)),
    m_normals(std::move(normals)),
    m_triangles(std::move(triangles))
{
    m_missing_norm_idx = m_normals.size();
    m_normals.push_back(vec4(0.0, 0.0, -1.0, 0.0));
    for (auto& tri : m_triangles) {
        // Pre-compute face normals for ray tracing purposes
        vec4 norm, p0, p1, p2;
        tri.tri_normal = m_normals.size();
        p0 = m_vertices[tri.vertices[0]];
        p1 = m_vertices[tri.vertices[1]];
        p2 = m_vertices[tri.vertices[2]];
        norm = vec4(glm::normalize(glm::cross(vec3(p1-p0), vec3(p2-p0))), 1.0);
        m_normals.push_back(norm);
        // Fill in missing normals
        for (int i = 0; i < 3; ++i) {
            if (tri.normals[i] < 0) {
                tri.normals[i] = m_missing_norm_idx;
            }
        }
    }
}

Model::triangle_iter Model::begin() const
{
    return Model::triangle_iter(*this, m_triangles.cbegin());
}

Model::triangle_iter Model::end() const
{
    return Model::triangle_iter(*this, m_triangles.cend());
}

Model::triangle_iter::triangle_iter(const Model& m, std::vector<FaceIndex>::const_iterator iter) :
    m_container(m),
    m_idx_iter(iter),
    m_current(m)
{
}

Model::triangle_iter& Model::triangle_iter::operator++()
{
    //  pre-inc
    ++m_idx_iter;
    return *this;
}

Model::triangle_iter Model::triangle_iter::operator++(int)
{
    // post-inc
    triangle_iter old(*this);
    m_idx_iter++;
    return old;
}

bool Model::triangle_iter::operator==(Model::triangle_iter& b) const
{
    return (&m_container == &b.m_container) && (m_idx_iter == b.m_idx_iter);
}

bool Model::triangle_iter::operator!=(Model::triangle_iter& b) const
{
    return !(*this == b);
}

Model::triangle& Model::triangle_iter::operator*()
{
    m_current.m_idx = &(*m_idx_iter);
    return m_current;
}

Model::triangle * Model::triangle_iter::operator->()
{
    m_current.m_idx = &(*m_idx_iter);
    return &m_current;
}

Model::triangle::triangle(const Model& m) :
    m_container(m),
    m_idx(nullptr)
{
}

vec4 Model::triangle::p0() const
{
    return m_container.get_vertices()[m_idx->vertices[0]];
}

vec4 Model::triangle::p1() const
{
    return m_container.get_vertices()[m_idx->vertices[1]];
}

vec4 Model::triangle::p2() const
{
    return m_container.get_vertices()[m_idx->vertices[2]];
}

vec4 Model::triangle::face_normal() const
{
    return m_container.get_normals()[m_idx->tri_normal];
}

vec4 Model::triangle::surface_normal(vec3 coords) const
{
    vec4 n0, n1, n2;
    auto& normals = m_container.get_normals();
    n0 = normals[m_idx->normals[0]];
    n1 = normals[m_idx->normals[1]];
    n2 = normals[m_idx->normals[2]];
    return coords.x * n0 + coords.y * n1 + coords.z * n2;
}

vec2 Model::triangle::surface_uvs(vec3 coords) const
{
    // STUB
    return vec2(0.0, 0.0);
}
