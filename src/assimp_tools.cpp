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
