#pragma once

#include "types.h"
#include <glm/vec3.hpp>

class Mesh;
class MeshInstance;

struct aabb {
    vec3 min, max;

    aabb() = default;

    /**
     * Compute an AABB around the mesh in object space.
     */
    aabb(const Mesh& m);

    /**
     * Compute an AABB around the object in world space.
     */
    aabb(const MeshInstance& o);

    vec3& operator[](size_t i)
    {
        if (i == 0) {
            return min;
        } else {
            return max;
        }
    }

    const vec3& operator[](size_t i) const
    {
        if (i == 0) {
            return min;
        } else {
            return max;
        }
    }
};
