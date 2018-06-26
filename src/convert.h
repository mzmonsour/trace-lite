#pragma once

#include "types.h"
#include <glm/mat4x4.hpp>
#include <assimp/matrix4x4.h>

/**
 * Convert an assimp matrix to the equivalent glm one. Required because assimp and glm use different
 * data orderings.
 */
inline mat4 assimp_mat_to_glm(const aiMatrix4x4& mat)
{
    return glm::mat4(   mat.a1, mat.a2, mat.a3, mat.a4,
                        mat.b1, mat.b2, mat.b3, mat.b4,
                        mat.c1, mat.c2, mat.c3, mat.c4,
                        mat.d1, mat.d2, mat.d3, mat.d4);
}
