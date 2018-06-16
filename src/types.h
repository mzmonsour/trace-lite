#pragma once

#include <glm/fwd.hpp>

#ifdef USE_DOUBLE_PRECISION
typedef double      scalar;
typedef glm::dvec2  vec2;
typedef glm::dvec3  vec3;
typedef glm::dvec4  vec4;
typedef glm::dmat2  mat2;
typedef glm::dmat3  mat3;
typedef glm::dmat4  mat4;
#else
typedef float       scalar;
typedef glm::vec2   vec2;
typedef glm::vec3   vec3;
typedef glm::vec4   vec4;
typedef glm::mat2   mat2;
typedef glm::mat3   mat3;
typedef glm::mat4   mat4;
#endif
