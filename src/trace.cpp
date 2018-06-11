#include "trace.h"
#include <glm/mat2x2.hpp>
#include <glm/geometric.hpp>
#include <limits>

trace_info trace_ray(const ray& r, const std::vector<Model>& objs)
{
    trace_info info;
    info.hitobj = nullptr;
    float best_d = std::numeric_limits<float>::infinity();
    for (auto& obj : objs) {
        for (auto& tri : obj) {
            float d;
            glm::vec4 p0, p1, p2, plane, pout;
            p0 = tri.p0();
            p1 = tri.p1();
            p2 = tri.p2();
            // Compute plane intersection
            // TODO Should norm be calculated, or provided?
            glm::vec3 norm = glm::cross(glm::vec3(p1-p0), glm::vec3(p2-p0));
            plane = glm::vec4(norm, -glm::dot(norm, glm::vec3(p0)));
            d = -glm::dot(plane, r.origin) / glm::dot(plane, r.dir);
            if (d < 0 || d >= best_d) {
                // Plane intersects behind ray, or lies behind our best trace
                continue;
            }
            pout = r.origin + d * r.dir;
            // Compute barycenter
            glm::vec4 r, q1, q2;
            float q1q1, q1q2, q2q2;
            glm::vec2 w, rq;
            glm::mat2 qmat;
            r = pout - p0;
            q1 = p1 - p0;
            q2 = p2 - p0;
            q1q1 = glm::dot(q1, q1);
            q2q2 = glm::dot(q2, q2);
            q1q2 = glm::dot(q1, q2);
            rq = glm::vec2(glm::dot(r,q1), glm::dot(r,q2));
            qmat[0][0] = q2q2;
            qmat[0][1] = -q1q2;
            qmat[1][0] = -q1q2;
            qmat[1][1] = q1q1;
            qmat =  (1.0f / (q1q1 * q2q2 - q1q2 * q1q2)) * qmat;
            w = qmat * rq;
            if (w.x >= 0.0f && w.y >= 0.0f && w.x + w.y <= 1.0f) {
                // Barycenter is valid; Point lies within triangle
                info.hitobj = &obj;
                info.hitpos = pout;
                info.barycenter = glm::vec3(1.0f - w.x - w.y, w.x, w.y);
                // TODO Return this, or computed normal?
                info.hitnorm = tri.surface_normal(info.barycenter);
                best_d = d;
            }
        }
    }
    return info;
}
