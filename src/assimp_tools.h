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

#include "types.h"
#include <assimp/scene.h>
#include <string>

/**
 * Log the structure of the scene graph.
 */
void log_assimp_scene_graph(const aiScene& scene);

/**
 * Search the assimp scene graph for a node, applying the transform given by the hierarchy.
 *
 * @param scene Scene graph
 * @param name Name of the node
 * @param xform_out The transform given by the node hierarchy is applied to this parameter
 *
 * @return The first node with the given name, or nullptr if no such node exists.
 */
const aiNode * search_assimp_scene_graph(const aiScene& scene, const aiString& name, mat4& xform_out);
const aiNode * search_assimp_scene_graph(const aiScene& scene, const std::string& name, mat4& xform_out);
