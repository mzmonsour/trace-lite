#pragma once

#include "model.h"

#include <glm/vec3.hpp>

struct ray {
    glm::vec3 origin;
    glm::vec3 dir;
};

struct trace_info
{
    glm::vec3   hitpos;
    glm::vec3   hitnorm;
    Model*      hitobj;
};

trace_info trace_ray(const ray& r, const std::vector<Model>& objs);
