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

#include "scene.h"
#include <iostream>
#include <iomanip>
#include <assimp/config.h>
#include <assimp/postprocess.h>

Scene::Scene() : m_scene(nullptr) {}

Scene::Scene(std::string& file)
{
    m_data.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    m_scene = m_data.ReadFile(file,
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_SortByPType);
    if (m_scene != nullptr) {
        process_meshes();
    }
}

void Scene::process_meshes()
{
    m_mesh_list.reserve(m_scene->mNumMeshes);
    for (unsigned int i = 0; i < m_scene->mNumMeshes; ++i) {
        Mesh m;
        try {
            m = Mesh(*m_scene->mMeshes[i]);
        } catch (std::invalid_argument& ex) {
            std::cout << "Processing of mesh "
                << std::quoted(m_scene->mMeshes[i]->mName.C_Str())
                << " failed" << std::endl;
            m = Mesh(); // If mesh processing fails, make an empty placeholder mesh.
        }
        m_mesh_list.emplace_back(std::move(m));
    }
}
