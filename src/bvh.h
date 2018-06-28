#pragma once

#include "types.h"
#include "mesh.h"
#include "aabb.h"
#include "scene.h"
#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>

struct trace_info;
class Ray;

class BVNode;

/**
 * Bounding Volume Hierarchy. Provides an efficient hierarchy for testing ray intersections in
 * complex scenes.
 * 
 */
class BVH {
    private:

        std::shared_ptr<BVNode> m_root;

    public:

        /**
         * Constructs a BVH given a scene graph.
         */
        BVH(const Scene& scene_graph);

        /**
         * Trace a ray into the BVH. If the ray intersects with any objects in the scene, information
         * about the first intersection will be returned. See trace_info for more info.
         */
        trace_info trace_ray(const Ray& r) const;

};

/**
 * Node in BVH. Represents a bounding volume in the hierarchy. Leaf nodes have an attached
 * MeshInstance.
 */
class BVNode {
    private:

        friend class BVH;

        aabb m_volume; // TODO Provide different BV options?
        std::unique_ptr<MeshInstance> m_obj;
        std::shared_ptr<BVNode> m_left, m_right;

    public:

        /**
         * Construct an internal node with the given volume and children.
         */
        BVNode(aabb volume, std::shared_ptr<BVNode> left, std::shared_ptr<BVNode> right);

        /**
         * Construct a leaf node with the given mesh instance and bounding volume.
         * 
         * @param obj unique_ptr to a MeshInstance, which is moved into the BVNode.
         */
        BVNode(aabb volume, std::unique_ptr<MeshInstance>& obj);

        /**
         * Check if the BVNode is a leaf node.
         */
        bool is_leaf() const { return m_obj != nullptr; }

        /**
         * Get the bounding volume associated with this node.
         */
        const aabb& bounding_volume() const { return m_volume; }

        /** 
         * Get the MeshInstance associated with this node. If the node is not a leaf, it will return
         * nullptr.
         */
        const MeshInstance* object() const { return m_obj.get(); }

};
