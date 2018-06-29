#include "trace.h"
#include "const.h"
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>
#include <algorithm>

const static scalar ERROR_THOLD = 0.00005;

Ray::Ray(vec4 origin, vec4 dir) :
    origin(origin),
    dir(dir) {}

scalar Ray::intersect_aabb(const aabb& volume) const
{
    scalar tmin, tmax;
    tmin = -SCALAR_INF;
    tmax = SCALAR_INF;
    vec3 half = (volume.max - volume.min) * (scalar)0.5;
    vec3 center = (volume.max + volume.min) * (scalar)0.5;
    vec3 p = vec3(this->origin) - center;
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
                return -SCALAR_INF;
            }
            if (tmax < 0) {
                // Intersection lies behind ray
                return -SCALAR_INF;
            }
        } else if (-e - half[c] > 0 || -e + half[c] < 0) {
            // Ray lies parallel, but not within the AABB
            return -SCALAR_INF;
        }
    }
    if (tmin > 0) {
        return tmin;
    } else {
        // Ray is inside AABB; return origin, or tmax?
        return 0;
    }
}

trace_info Ray::intersect_mesh(const MeshInstance& mesh) const
{
    // STUB
    trace_info info;
    info.hitobj = nullptr;
    return info;
}

trace_info trace_ray(const ray& r, const std::vector<Model>& objs)
{
    trace_info info;
    info.hitobj = nullptr;
    scalar best_d = std::numeric_limits<scalar>::infinity();
    for (auto& obj : objs) {
        for (auto& tri : obj) {
            scalar d;
            vec4 p0, p1, p2, plane, pout;
            p0 = tri.p0();
            p1 = tri.p1();
            p2 = tri.p2();
            // Compute plane intersection
            vec3 norm = tri.face_normal();
            plane = vec4(norm, -glm::dot(norm, vec3(p0)));
            d = -glm::dot(plane, r.origin) / glm::dot(plane, r.dir);
            if (d < 0 || d >= best_d) {
                // Plane intersects behind ray, or lies behind our best trace
                continue;
            }
            pout = r.origin + d * r.dir;

            // Compute barycenter
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
                info.hitobj = (const MeshInstance*)&obj; // TODO Remove spooky cast
                info.hitpos = pout;
                info.barycenter = vec3((scalar)1.0 - w.x - w.y, w.x, w.y);
                // TODO Return this, or computed normal?
                info.hitnorm = tri.surface_normal(info.barycenter);
                best_d = d;
            }
        }
    }
    return info;
}
