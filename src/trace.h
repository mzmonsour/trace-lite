#pragma once

#include "model.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

struct ray {
    glm::vec4 origin;
    glm::vec4 dir;
};

struct trace_info
{
    const Model*    hitobj;
    glm::vec4       hitpos;
    glm::vec4       hitnorm;
    glm::vec3       barycenter;
};

trace_info trace_ray(const ray& r, const std::vector<Model>& objs);
