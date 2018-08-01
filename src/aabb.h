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
