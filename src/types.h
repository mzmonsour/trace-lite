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
