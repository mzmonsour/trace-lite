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

#include "mesh.h"
#include "convert.h"
#include <stdexcept>
#include <glm/geometric.hpp>

Mesh::Mesh() {}

Mesh::Mesh(const aiMesh& mesh) :
    m_name(mesh.mName.C_Str())
{
    m_faces.reserve(mesh.mNumFaces);
    m_vertices.reserve(mesh.mNumVertices);
    m_plane_normals.reserve(mesh.mNumFaces);
    if (mesh.mNormals != nullptr) {
        m_normals.reserve(mesh.mNumVertices);
    }
    if (mesh.mTextureCoords[0] != nullptr) {
        m_uvs.reserve(mesh.mNumVertices);
    }
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        m_vertices.emplace_back(assimp_vec_to_glm4(mesh.mVertices[i], 1.0));
        if (mesh.mNormals != nullptr) {
            m_normals.emplace_back(assimp_vec_to_glm4(mesh.mNormals[i], 0.0));
        }
        if (mesh.mTextureCoords[0] != nullptr) {
            m_uvs.emplace_back(assimp_vec_to_glm3(mesh.mTextureCoords[0][i]));
        }
    }
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i) {
        if (mesh.mFaces[i].mNumIndices != 3) {
            throw std::invalid_argument("Mesh must be fully triangulated");
        }
        struct face f;
        f.index[0] = mesh.mFaces[i].mIndices[0];
        f.index[1] = mesh.mFaces[i].mIndices[1];
        f.index[2] = mesh.mFaces[i].mIndices[2];
        m_faces.push_back(f);
        // Compute plane normal
        auto& p0 = m_vertices[f.index[0]];
        auto& p1 = m_vertices[f.index[1]];
        auto& p2 = m_vertices[f.index[2]];
        vec4 pnorm = vec4(glm::normalize(glm::cross(vec3(p1-p0), vec3(p2-p0))), 0.0);
        m_plane_normals.emplace_back(pnorm);
    }
    m_aabb = aabb(*this);
}

MeshInstance::MeshInstance(const Mesh& mesh, mat4 xform) :
    m_mesh(mesh),
    m_xform(xform),
    m_inv_xform(glm::inverse(xform)) {}
