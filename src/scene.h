#pragma once

#include "mesh.h"
#include <assimp/scene.h>

class Scene {
    private:

        aiScene *m_scene;
        std::vector<Mesh> m_mesh_list;

    public:

        /**
         * Construct an empty scene.
         */
        Scene();

        /**
         * Construct a scene from an imported assimp scene graph.
         */
        Scene(aiScene& scene);

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
