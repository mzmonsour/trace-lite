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
