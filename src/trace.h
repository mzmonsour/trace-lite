#pragma once

#include "mesh.h"
#include "types.h"
#include "model.h"
#include "bvh.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

enum struct IntersectionType {
    None, /// No intersection occurred.
    Intersected, /// Trace intersected volume.
    BehindRay, // Intersection occurred behind ray.
    InsideVolume, /// Ray origin lies inside volume.
    Degenerate, /// Intersection occurred across volume boundary.
};

/**
 * Detailed information about a trace.
 */
struct trace_info
{
    /** Type of intersection that occurred. If set to None, additional state may be undefined. */
    IntersectionType intersect_type;
    /** MeshInstance hit by the trace. */
    const MeshInstance *hitobj;
    /** Position of the intersection. */
    vec4 hitpos;
    /** Normal at the point of intersection. */
    vec4 hitnorm;
    /** Barycenter of the intersection on the triangle. */
    vec3 barycenter;
    /** Distance of the intersection along the ray. */
    scalar distance;
};

/**
 * Basic information about a trace.
 */
struct trace_result
{
    IntersectionType intersect_type;
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
        trace_result intersect_aabb(const aabb& volume) const;

        /**
         * Test complex intersection vs a MeshInstance. Gives detailed information about the first
         * intersection along the ray. See trace_info for more info.
         */
        trace_info intersect_mesh(const MeshInstance& obj) const;

};
