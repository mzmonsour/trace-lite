#include "const.h"
#include "convert.h"
#include "bvh.h"
#include "trace.h"
#include <algorithm>

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

/**
 * Build the leaves of the BVH from objects in the scene recursively.
 */
static void build_bvh_leaves(  std::vector<std::shared_ptr<BVNode>>& leaves,
                                const Scene& scene_graph, const aiNode* node,
                                const mat4& xform)
{
    mat4 this_xform = xform * assimp_mat_to_glm(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const Mesh& mesh = scene_graph.mesh_list()[node->mMeshes[i]];
        auto instance = std::make_unique<MeshInstance>(mesh, this_xform);
        leaves.emplace_back(std::make_shared<BVNode>(aabb(*instance), instance));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        build_bvh_leaves(leaves, scene_graph, node->mChildren[i], this_xform);
    }
}

struct axis_sort {
    /** Axis index */
    const int c;

    axis_sort(int c) : c(c) {}

    inline bool operator()(const std::shared_ptr<BVNode>& a, const std::shared_ptr<BVNode>& b) const
    {
        scalar mida, midb;
        auto& bva = a->bounding_volume();
        auto& bvb = b->bounding_volume();
        mida = (bva.min[c] + bva.max[c]) * 0.5;
        midb = (bvb.min[c] + bvb.max[c]) * 0.5;
        return mida < midb;
    }
};

typedef std::vector<std::shared_ptr<BVNode>>::iterator bvn_iter;

/**
 * Build the BVH tree in a top down manner, recursively.
 */
static std::shared_ptr<BVNode> build_bvh_topdown(bvn_iter begin, bvn_iter end)
{
    if (begin == end) {
        return nullptr;
    }
    size_t itersize = std::distance(begin, end);
    if (itersize == 1) {
        return *begin;
    }
    // Compute node AABB
    aabb box;
    box.min = VEC3_MAXIMUM;
    box.max = VEC3_MINIMUM;
    for (auto& it = begin; it != end; ++it) {
        auto& bv = (*it)->bounding_volume();
        for (int c = 0; c < 3; ++c) {
            if (bv.min[c] < box.min[c]) {
                box.min[c] = bv.min[c];
            }
            if (bv.max[c] > box.max[c]) {
                box.max[c] = bv.max[c];
            }
        }
    }
    // Compute longest axis
    vec3 length = box.max - box.min;
    scalar max_axis = length.x;
    int max_idx = 0;
    for (int c = 1; c < 3; ++c) {
        if (length[c] > max_axis) {
            max_axis = length[c];
            max_idx = c;
        }
    }
    // Sort iterator range along longest axis
    std::sort(begin, end, axis_sort(max_idx));
    // Split iterator range in half and recurse
    auto half = begin + itersize/2;
    auto left = build_bvh_topdown(begin, half);
    auto right = build_bvh_topdown(half, end);
    return std::make_shared<BVNode>(box, left, right);
}

BVH::BVH(const Scene& scene_graph)
{
    std::vector<std::shared_ptr<BVNode>> leaves;
    build_bvh_leaves(leaves, scene_graph, scene_graph.assimp_scene()->mRootNode, MAT4_IDENTITY);
    std::cout << "BVH: Instanced " << leaves.size() << " meshes from scene graph" << std::endl;
    for (auto& leaf : leaves) {
        std::cout << "\tAABB Extents:"
            << " min=" << glm::to_string(leaf->m_volume.min)
            << " max=" << glm::to_string(leaf->m_volume.max)
            << std::endl;
    }
    m_root = build_bvh_topdown(leaves.begin(), leaves.end());
}

trace_info BVH::trace_ray(const Ray& r) const
{
    struct leaf_trace {
        BVNode *leaf;
        scalar dist;
        leaf_trace(BVNode *n, scalar d) :
            leaf(n), dist(d) {}
    };
    trace_info info;
    info.hitobj = nullptr;
    std::vector<BVNode*> to_search;
    std::vector<leaf_trace> hit_leaves;
    if (m_root != nullptr) {
        to_search.push_back(m_root.get());
    }
    // Trace through tree, pruning branches as needed
    while (!to_search.empty()) {
        BVNode *n = to_search.back();
        to_search.pop_back();
        trace_result result = r.intersect_aabb(n->bounding_volume());
        if (result.intersect_type == IntersectionType::Intersected || result.intersect_type == IntersectionType::InsideVolume) {
            //std::cout << "AABB HIT\n";
            if (n->is_leaf()) {
                hit_leaves.emplace_back(n, result.distance);
            } else {
                if (n->m_left != nullptr) {
                    to_search.push_back(n->m_left.get());
                }
                if (n->m_right != nullptr) {
                    to_search.push_back(n->m_right.get());
                }
            }
        } else {
            switch (result.intersect_type) {
                case IntersectionType::None: {
                    //std::cout << "(NONE)\n";
                } break;
                case IntersectionType::BehindRay: {
                    //std::cout << "(BEHIND)\n";
                } break;
                default: {
                    //std::cout << "(UNKNOWN)\n";
                }
            }
        }
    }
    // Find closest intersection
    std::sort(hit_leaves.begin(), hit_leaves.end(),
            [](const auto& a, const auto& b) { return a.dist < b.dist; });
    info.distance = SCALAR_INF;
    for (auto& lt : hit_leaves) {
        if (info.distance < lt.dist) {
            // Early stop, we already know the best possible trace
            break;
        }
        struct trace_info temp = r.intersect_mesh(*(lt.leaf->object()));
        if (temp.hitobj != nullptr && temp.distance < info.distance) {
            info = temp;
        }
    }
    return info;
}

BVNode::BVNode(aabb volume, std::shared_ptr<BVNode> left, std::shared_ptr<BVNode> right) :
    m_volume(volume),
    m_obj(nullptr),
    m_left(left),
    m_right(right) {}

BVNode::BVNode(aabb volume, std::unique_ptr<MeshInstance>& obj) :
    m_volume(volume),
    m_obj(std::move(obj)) {}
