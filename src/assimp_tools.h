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
