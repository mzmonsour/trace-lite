#include "render.h"
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <iostream>

glm::vec3 linear_to_srgb(glm::vec3 color) {
    // TODO More accurate sRGB correction
    glm::vec3 out;
    out.x = glm::pow(color.x, 1.0/2.2);
    out.y = glm::pow(color.y, 1.0/2.2);
    out.z = glm::pow(color.z, 1.0/2.2);
    return glm::clamp(out, 0.0f, 1.0f);
}

Camera::Camera(glm::mat4x4 xform, float fov, float aspect) :
    m_xform(glm::inverse(xform)),
    m_fov(fov),
    m_aspect(aspect)
{
}

ray Camera::compute_ray(glm::vec2 pos) const
{
    ray out;
    float x, y;
    float tan = glm::tan(m_fov * 0.5);
    x = tan * m_aspect * pos.x;
    y = tan * pos.y;
    out.dir = glm::normalize(m_xform * glm::vec4(x, y, -1.0, 0.0));
    out.origin = m_xform * glm::vec4(0.0, 0.0, 0.0, 1.0); // TODO Cache origin?
    return out;
}

Scene::Scene(const std::vector<Model>& objects) :
    m_objects(objects)
{
}

std::vector<rgb_color> Scene::render(Camera& cam, render_options opts) const
{
    std::vector<rgb_color> img;
    img.reserve(opts.width * opts.height);
    size_t progress = 0, percent = 0;
    size_t hits = 0;
    std::cout << "Rendering" << std::flush;
    for (int y = 0; y < opts.height; ++y) {
        for (int x = 0; x < opts.width; ++x) {
            progress++;
            ray view_ray = cam.compute_ray(
                    glm::vec2(  2.0 * ((float)x) / ((float)opts.width) - 1.0,
                                1.0 - 2.0 * ((float)y) / ((float)opts.height)));
            trace_info trace = trace_ray(view_ray, m_objects);
            glm::vec3 color(0.0, 0.0, 0.0);
            if (trace.hitobj != nullptr) {
                // TODO Shading and materials
                if (opts.debug_flags & debug_mode::normal_coloring) {
                    color = glm::vec3(trace.hitnorm);
                } else {
                    color = glm::vec3(0.7, 0.7, 0.7);
                }
                hits++;
            }
            color = linear_to_srgb(color);
            rgb_color imgcolor;
            imgcolor.r = color.r * 255;
            imgcolor.g = color.g * 255;
            imgcolor.b = color.b * 255;
            img.push_back(imgcolor);
        }
        if ((progress * 100) / (opts.width * opts.height) > percent) {
            percent++;
            std::cout << "." << std::flush;
        }
    }
    std::cout << "done!" << std::endl;
    std::cout << "Total ray hits: " << hits << std::endl;
    return img;
}
