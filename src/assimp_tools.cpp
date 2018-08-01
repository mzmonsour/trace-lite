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

#include "assimp_tools.h"
#include "convert.h"
#include <vector>
#include <iostream>

struct node_path {
    const aiNode *node;
    size_t next_child;

    node_path(const aiNode *node) :
        node(node),
        next_child(0) {}
};

static void log_assimp_node(const aiNode *node, size_t level) {
    for (size_t i = 0; i < level; ++i) {
        std::cout << ' ';
    }
    std::cout << "> " << node->mName.C_Str() << std::endl;
    for (size_t i = 0; i < node->mNumChildren; ++i) {
        log_assimp_node(node->mChildren[i], level + 1);
    }
}

void log_assimp_scene_graph(const aiScene& scene) {
    log_assimp_node(scene.mRootNode, 0);
}

const aiNode * search_assimp_scene_graph(const aiScene& scene, const aiString& name, mat4& xform_out)
{
    std::vector<node_path> pathstack;
    if (scene.mRootNode == nullptr) {
        return nullptr;
    }
    pathstack.emplace_back(scene.mRootNode);
    while (!pathstack.empty()) {
        auto& top = pathstack.back();
        if (top.next_child == 0 && top.node->mName == name) {
            break;
        }
        if (top.next_child >= top.node->mNumChildren) {
            pathstack.pop_back();
            continue;
        }
        auto child = top.node->mChildren[top.next_child++];
        pathstack.emplace_back(child);
    }
    if (pathstack.empty()) {
        return nullptr;
    }
    for (auto& n : pathstack) {
        xform_out *= assimp_mat_to_glm(n.node->mTransformation);
    }
    return pathstack.back().node;
}

const aiNode * search_assimp_scene_graph(const aiScene& scene, const std::string& name, mat4& xform_out)
{
    return search_assimp_scene_graph(scene, aiString(name), xform_out);
}
