#include "scene.h"
#include <iostream>
#include <iomanip>

Scene::Scene() : m_scene(nullptr) {}

Scene::Scene(aiScene& scene) : m_scene(&scene)
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
