#pragma once

#include "mesh.h"
#include "types.h"
#include "model.h"
#include "bvh.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

/**
 * Gives information about the result of a trace.
 */
struct trace_info
{
    /** MeshInstance hit by the trace. Set to nullptr if trace hit nothing. */
    const MeshInstance* hitobj;
    /** Position of the intersection. */
    vec4 hitpos;
    /** Normal at the point of intersection. */
    vec4 hitnorm;
    /** Barycenter of the intersection on the triangle. */
    vec3 barycenter;
    /** Distance of the intersection along the ray. */
    scalar distance;
};

class Ray {
    public:

        vec4 origin;
        vec4 dir;

        Ray() = default;

        Ray(vec4 origin, vec4 dir);

        /**
         * Test intersection vs an AABB.
         *
         * @return Distance of the first intersection along the ray. If the distance is negative,
         * the ray does not intersect.
         */
        scalar intersect_aabb(const aabb& volume) const;

        /**
         * Test complex intersection vs a MeshInstance. Gives detailed information about the first
         * intersection along the ray. See trace_info for more info.
         */
        trace_info intersect_mesh(const MeshInstance& mesh) const;

};

struct ray {
    vec4 origin;
    vec4 dir;
};

trace_info trace_ray(const ray& r, const std::vector<Model>& objs);
