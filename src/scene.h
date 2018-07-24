#pragma once

#include "mesh.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Scene {
    private:

        Assimp::Importer m_data;
        const aiScene *m_scene;
        std::vector<Mesh> m_mesh_list;

        void process_meshes();

    public:

        /**
         * Construct an empty scene.
         */
        Scene();

        /**
         * Load a scene from a file.
         */
        Scene(std::string& file);

        /**
         * Get the assimp scene backing this scene.
         */
        const aiScene * assimp_scene() const { return m_scene; }

        /**
         * Get the list of pre-processed meshes in this scene. Each mesh corresponds to the aiMesh
         * in the scene at the same index.
         */
        const std::vector<Mesh>& mesh_list() const { return m_mesh_list; }

};
