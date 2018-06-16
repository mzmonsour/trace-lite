#pragma once

#include "types.h"
#include "model.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

struct ray {
    vec4 origin;
    vec4 dir;
};

struct trace_info
{
    const Model*    hitobj;
    vec4            hitpos;
    vec4            hitnorm;
    vec3            barycenter;
};

trace_info trace_ray(const ray& r, const std::vector<Model>& objs);
