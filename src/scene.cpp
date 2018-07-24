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
