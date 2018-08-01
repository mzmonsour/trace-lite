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
