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

#include "trace.h"
#include "const.h"
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>
#include <algorithm>

const static scalar ERROR_THOLD = 0.00005;

Ray::Ray(vec4 origin, vec4 dir) :
    origin(origin),
    dir(dir) {}

trace_result Ray::intersect_aabb(const aabb& volume) const
{
    trace_result result;
    result.distance = -SCALAR_INF;
    scalar tmin, tmax;
    tmin = -SCALAR_INF;
    tmax = SCALAR_INF;
    vec3 half = (volume.max - volume.min) * (scalar)0.5;
    vec3 center = (volume.max + volume.min) * (scalar)0.5;
    vec3 p = center - vec3(this->origin);
    for (int c = 0; c < 3; ++c) {
        scalar e = p[c];
        scalar f = this->dir[c];
        // TODO Pick an epsilon to negate precision errors
        if (glm::abs(f) > 0) {
            scalar inv_f = 1/f;
            scalar t1 = (e + half[c]) * inv_f;
            scalar t2 = (e - half[c]) * inv_f;
            if (t1 > t2) {
                std::swap(t1, t2);
            }
            if (t1 > tmin) {
                tmin = t1;
            }
            if (t2 < tmax) {
                tmax = t2;
            }
            if (tmin > tmax) {
                // Ray misses AABB
                result.intersect_type = IntersectionType::None;
                return result;
            }
            if (tmax < 0) {
                // Intersection lies behind ray
                result.intersect_type = IntersectionType::BehindRay;
                return result;
            }
        } else if (-e - half[c] > 0 || -e + half[c] < 0) {
            // Ray lies parallel, but not within the AABB
            result.intersect_type = IntersectionType::None;
            return result;
        }
    }
    if (tmin > 0) {
        result.intersect_type = IntersectionType::Intersected;
        result.distance = tmin;
    } else {
        // Ray is inside AABB; return origin, or tmax?
        result.intersect_type = IntersectionType::InsideVolume;
        result.distance = 0;
    }
    return result;
}

trace_info Ray::intersect_mesh(const MeshInstance& obj) const
{
    trace_info info;
    info.intersect_type = IntersectionType::None;
    info.hitobj = nullptr;
    //scalar best_d = SCALAR_INF;
    auto& to_world = obj.transform();
    auto& to_obj = obj.inverse_transform();
    // Cast ray in object space
    vec4 origin = to_obj * this->origin;
    vec4 dir = to_obj * this->dir;
    info.distance = SCALAR_INF;
    for (auto& tri : obj.mesh().triangles()) {
        scalar t;
        auto& p0 = tri.p0();
        auto& p1 = tri.p1();
        auto& p2 = tri.p2();
        auto& norm = tri.plane_normal();
        // Compute plane intersection
        vec4 plane(vec3(norm), -glm::dot(norm, p0));
        t = -glm::dot(plane, origin) / glm::dot(plane, dir);
        if (t < 0 || t > info.distance) {
            // Plane intersects behind ray, or lies behind best trace
            continue;
        }
        // Compute barycenter
        vec4 pout = origin + t * dir;
        vec4 r, q1, q2;
        scalar q1q1, q1q2, q2q2;
        vec2 w, rq;
        mat2 qmat;
        r = pout - p0;
        q1 = p1 - p0;
        q2 = p2 - p0;
        q1q1 = glm::dot(q1, q1);
        q2q2 = glm::dot(q2, q2);
        q1q2 = glm::dot(q1, q2);
        rq = vec2(glm::dot(r,q1), glm::dot(r,q2));
        qmat[0][0] = q2q2;
        qmat[0][1] = -q1q2;
        qmat[1][0] = -q1q2;
        qmat[1][1] = q1q1;
        qmat =  (((scalar)1.0) / (q1q1 * q2q2 - q1q2 * q1q2)) * qmat;
        w = qmat * rq;
        if (w.x >= 0.0 - ERROR_THOLD && w.y >= 0.0 - ERROR_THOLD && w.x + w.y <= 1.0 + ERROR_THOLD) {
            // Barycenter is valid; Point lies within triangle
            info.intersect_type = IntersectionType::Intersected;
            info.hitobj = &obj;
            info.hitpos = pout;
            info.barycenter = vec3((scalar)1.0 - w.x - w.y, w.x, w.y);
            info.hitnorm = tri.surface_normal(info.barycenter);
            info.distance = t;
        }
    }
    info.hitpos = to_world * info.hitpos;
    info.hitnorm = glm::normalize(to_world * info.hitnorm);
    return info;
}
