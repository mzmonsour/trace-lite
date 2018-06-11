#include "render.h"
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <iostream>

render_options::render_options() :
    width(640),
    height(480),
    debug_flags(debug_mode::none),
    msaa(false)
{
}

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
            size_t samplecount, msfactor = 1;
            if (opts.msaa) {
                msfactor = 2;
            }
            std::vector<glm::vec3> samples;
            samplecount = msfactor * msfactor;
            for (size_t s = 0; s < samplecount; ++s) {
                float sx, sy;
                sx = (float)(s % msfactor);
                sy = (float)(s / msfactor);
                ray view_ray = cam.compute_ray(
                        glm::vec2(  2.0 * (((float)x*msfactor) + sx) / ((float)opts.width * msfactor) - 1.0,
                                    1.0 - 2.0 * (((float)y*msfactor) + sy) / ((float)opts.height * msfactor)));
                trace_info trace = trace_ray(view_ray, m_objects);
                glm::vec3 sample(0.0, 0.0, 0.0);
                if (trace.hitobj != nullptr) {
                    // TODO Shading and materials
                    if (opts.debug_flags & debug_mode::normal_coloring) {
                        sample = glm::vec3(trace.hitnorm);
                    } else if (opts.debug_flags & debug_mode::interp_coloring) {
                        sample = glm::vec3(trace.barycenter);
                    } else {
                        sample = glm::vec3(0.7, 0.7, 0.7);
                    }
                    hits++;
                }
                samples.push_back(sample);
            }
            glm::vec3 color(0.0, 0.0, 0.0);
            for (auto& s : samples) {
                color += s;
            }
            color *= 1.0f/((float)samples.size());
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
