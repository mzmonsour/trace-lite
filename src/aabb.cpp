#include "const.h"
#include "aabb.h"
#include "mesh.h"

aabb::aabb(const Mesh& m)
{
    if (!m.vertices().empty()) {
        this->max = VEC3_MINIMUM;
        this->min = VEC3_MAXIMUM;
        for (auto& v : m.vertices()) {
            for (int c = 0; c < 3; ++c) {
                if (v[c] < this->min[c]) {
                    this->min[c] = v[c];
                }
                if (v[c] > this->max[c]) {
                    this->max[c] = v[c];
                }
            }
        }
    }
}

aabb::aabb(const MeshInstance& o)
{
    const aabb& object = o.mesh().object_space_aabb();
    this->max = VEC3_MINIMUM;
    this->min = VEC3_MAXIMUM;
    vec3 obb[8];
    auto& xform = o.transform();
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
            if (obb[i][c] < this->min[c]) {
                this->min[c] = obb[i][c];
            }
            if (obb[i][c] > this->max[c]) {
                this->max[c] = obb[i][c];
            }
        }
    }
}
