/*
 * Copyright (c) 2018 Matt Monsour
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <glm/geometric.hpp>

inline Mesh::TriangleIter Mesh::TriangleContainer::begin() const
{
    return TriangleIter(&m_mesh, 0);
}

inline Mesh::TriangleIter Mesh::TriangleContainer::end() const
{
    return TriangleIter(&m_mesh, m_mesh.faces().size());
}

inline Mesh::TriangleIter& Mesh::TriangleIter::operator++()
{
    m_face.m_face++;
    return *this;
}

inline Mesh::TriangleIter Mesh::TriangleIter::operator++(int)
{
    TriangleIter it = *this;
    m_face.m_face++;
    return it;
}

inline bool Mesh::TriangleIter::operator==(const Mesh::TriangleIter& b) const
{
    return m_face == b.m_face;
}

inline bool Mesh::TriangleIter::operator!=(const Mesh::TriangleIter& b) const
{
    return m_face != b.m_face;
}

inline Mesh::Triangle& Mesh::TriangleIter::operator*()
{
    return m_face;
}

inline Mesh::Triangle * Mesh::TriangleIter::operator->()
{
    return &m_face;
}

inline bool Mesh::Triangle::operator==(const Triangle& b) const
{
    return m_mesh == b.m_mesh && m_face == b.m_face;
}

inline bool Mesh::Triangle::operator!=(const Triangle& b) const
{
    return !(*this == b);
}

inline const vec4& Mesh::Triangle::p0() const
{
    auto& f = m_mesh->faces()[m_face];
    return m_mesh->vertices()[f.index[0]];
}

inline const vec4& Mesh::Triangle::p1() const
{
    auto& f = m_mesh->faces()[m_face];
    return m_mesh->vertices()[f.index[1]];
}

inline const vec4& Mesh::Triangle::p2() const
{
    auto& f = m_mesh->faces()[m_face];
    return m_mesh->vertices()[f.index[2]];
}

inline const vec4& Mesh::Triangle::plane_normal() const
{
    return m_mesh->plane_normals()[m_face];
}

inline vec4 Mesh::Triangle::surface_normal(vec3 coords) const
{
    if (m_mesh->normals().empty()) {
        // TODO: Missing normal behavior?
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    auto i0 = m_mesh->faces()[m_face].index[0];
    auto i1 = m_mesh->faces()[m_face].index[1];
    auto i2 = m_mesh->faces()[m_face].index[2];
    auto& n0 = m_mesh->normals()[i0];
    auto& n1 = m_mesh->normals()[i1];
    auto& n2 = m_mesh->normals()[i2];
    return (coords.x * n0) + (coords.y * n1) + (coords.z * n2);
}

inline vec2 Mesh::Triangle::surface_uvs(vec3 coords) const
{
    if (m_mesh->uv_coordinates().empty()) {
        // TODO: Missing normal behavior?
        return vec2(0.0, 0.0);
    }
    auto i0 = m_mesh->faces()[m_face].index[0];
    auto i1 = m_mesh->faces()[m_face].index[1];
    auto i2 = m_mesh->faces()[m_face].index[2];
    auto& uv0 = m_mesh->uv_coordinates()[i0];
    auto& uv1 = m_mesh->uv_coordinates()[i1];
    auto& uv2 = m_mesh->uv_coordinates()[i2];
    return (coords.x * uv0) + (coords.y * uv1) + (coords.z * uv2);
}
