#include "types.h"
#include <limits>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const static scalar SCALAR_INF = std::numeric_limits<scalar>::infinity();

const static vec3 VEC3_MAXIMUM = vec3(SCALAR_INF, SCALAR_INF, SCALAR_INF);
const static vec3 VEC3_MINIMUM = -VEC3_MAXIMUM;

const static mat4 MAT4_IDENTITY = mat4((scalar)1.0);
