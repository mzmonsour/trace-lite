#include "const.h"
#include "convert.h"
#include "bvh.h"
#include "trace.h"

aabb compute_mesh_aabb(const aiMesh& mesh, const mat4& xform)
{
    aabb object, world;
    vec3 obb[8];
    world.max = object.max = VEC3_MINIMUM;
    world.min = object.min = VEC3_MAXIMUM;
    // Compute extents of AABB in object space
    // TODO: Object space AABB is computed per instance, perhaps we should cache this per mesh?
    for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
        for (int c = 0; c < 3; ++c) {
            if (mesh.mVertices[i][c] < object.min[c]) {
                object.min[c] = mesh.mVertices[i][c];
            }
            if (mesh.mVertices[i][c] > object.max[c]) {
                object.max[c] = mesh.mVertices[i][c];
            }
        }
    }
    // Transform object space AABB to world space OBB
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                obb[i*4 + j*2 + k] = vec3(xform * vec4(object[i].x, object[j].y, object[k].z, 1.0));
            }
        }
    }
    // Compute world space AABB from OBB
    for (unsigned int i = 0; i < 8; ++i) {
        for (int c = 0; c < 3; ++c) {
            if (obb[i][c] < world.min[c]) {
                world.min[c] = obb[i][c];
            }
            if (obb[i][c] > world.max[c]) {
                world.max[c] = obb[i][c];
            }
        }
    }
    return world;
}

/**
 * Build the leaves of the BVH from objects in the scene recursively.
 */
static void build_bvh_leaves(  std::vector<std::shared_ptr<BVNode>>& leaves,
                                aiScene* scene_graph, aiNode* node, const mat4& xform)
{
    mat4 this_xform = xform * assimp_mat_to_glm(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene_graph->mMeshes[node->mMeshes[i]];
        aabb volume = compute_mesh_aabb(*mesh, this_xform);
        auto instance = std::make_unique<MeshInstance>(mesh, this_xform);
        leaves.emplace_back(std::make_shared<BVNode>(volume, instance));
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

BVH::BVH(aiScene* scene_graph)
{
    std::vector<std::shared_ptr<BVNode>> leaves;
    build_bvh_leaves(leaves, scene_graph, scene_graph->mRootNode, mat4());
    m_root = build_bvh_topdown(leaves.begin(), leaves.end());
}

trace_info BVH::trace_ray(const Ray& r) const
{
    // STUB
    trace_info info;
    info.hitobj = nullptr;
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
