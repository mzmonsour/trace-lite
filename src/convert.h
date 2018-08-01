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
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

/**
 * Convert an assimp matrix to the equivalent glm one. Required because assimp and glm use different
 * data orderings.
 */
inline mat4 assimp_mat_to_glm(const aiMatrix4x4& mat)
{
    return glm::mat4(   mat.a1, mat.b1, mat.c1, mat.d1,
                        mat.a2, mat.b2, mat.c2, mat.d2,
                        mat.a3, mat.b3, mat.c3, mat.d3,
                        mat.a4, mat.b4, mat.c4, mat.d4);
}

inline vec4 assimp_vec_to_glm4(const aiVector3D& vec, scalar w)
{
    return vec4(vec.x, vec.y, vec.z, w);
}

inline vec3 assimp_vec_to_glm3(const aiVector3D& vec)
{
    return vec3(vec.x, vec.y, vec.z);
}
