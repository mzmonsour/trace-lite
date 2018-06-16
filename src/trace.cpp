#include "trace.h"
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>
#include <limits>

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
            // TODO Should norm be calculated, or provided?
            vec3 norm = glm::cross(vec3(p1-p0), vec3(p2-p0));
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
            if (w.x >= 0.0 && w.y >= 0.0 && w.x + w.y <= 1.0) {
                // Barycenter is valid; Point lies within triangle
                info.hitobj = &obj;
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
