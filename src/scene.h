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
