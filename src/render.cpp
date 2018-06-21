#include "render.h"
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

render_options::render_options() :
    width(640),
    height(480),
    debug_flags(debug_mode::none),
    msaa(false),
    max_recursion(1)
{
}

vec3 linear_to_srgb(vec3 color) {
    // TODO More accurate sRGB correction
    vec3 out;
    out.x = glm::pow(color.x, 1.0/2.2);
    out.y = glm::pow(color.y, 1.0/2.2);
    out.z = glm::pow(color.z, 1.0/2.2);
    return glm::clamp(out, (scalar)0.0, (scalar)1.0);
}

Camera::Camera(mat4 xform, scalar fov, scalar aspect) :
    m_xform(glm::inverse(xform)),
    m_fov(fov),
    m_aspect(aspect)
{
}

ray Camera::compute_ray(vec2 pos) const
{
    ray out;
    scalar x, y;
    scalar tan = glm::tan(m_fov * 0.5);
    x = tan * m_aspect * pos.x;
    y = tan * pos.y;
    out.dir = glm::normalize(m_xform * vec4(x, y, -1.0, 0.0));
    out.origin = m_xform * vec4(0.0, 0.0, 0.0, 1.0); // TODO Cache origin?
    return out;
}

Scene::Scene(const std::vector<Model>& objects, std::vector<std::unique_ptr<Light>> lights) :
    m_objects(objects),
    m_lights(std::move(lights))
{
}

std::vector<rgb_color> Scene::render(Camera& cam, render_options opts) const
{
    std::vector<rgb_color> img;
    img.reserve(opts.width * opts.height);
    size_t progress = 0, percent = 0;
    std::cout << "Rendering" << std::flush;
    for (int y = 0; y < opts.height; ++y) {
        for (int x = 0; x < opts.width; ++x) {
            progress++;
            size_t samplecount, msfactor = 1;
            if (opts.msaa) {
                msfactor = 2;
            }
            std::vector<vec3> samples;
            samplecount = msfactor * msfactor;
            for (size_t s = 0; s < samplecount; ++s) {
                scalar sx, sy;
                sx = (scalar)(s % msfactor);
                sy = (scalar)(s / msfactor);
                ray view_ray = cam.compute_ray(
                        vec2(  2.0 * (((scalar)x*msfactor) + sx) / ((scalar)opts.width * msfactor) - 1.0,
                                    1.0 - 2.0 * (((scalar)y*msfactor) + sy) / ((scalar)opts.height * msfactor)));
                vec3 sample = this->compute_ray_color(view_ray, opts, opts.max_recursion);
                samples.push_back(sample);
            }
            vec3 color(0.0, 0.0, 0.0);
            for (auto& s : samples) {
                color += s;
            }
            color *= 1.0/((scalar)samples.size());
            // Disable sRGB conversion when using debug color modes
            if (!(opts.debug_flags & debug_mode::normal_coloring)
                    && !(opts.debug_flags & debug_mode::interp_coloring)) {
                color = linear_to_srgb(color);
            }
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
    return img;
}

vec3 Scene::compute_ray_color(const ray& r, const render_options& opts, size_t steps) const
{
    vec3 color(0.0, 0.0, 0.0);
    if (steps == 0) {
        return color;
    }
    trace_info trace = trace_ray(r, m_objects);
    if (trace.hitobj != nullptr) {
            vec4 v, n; // View dir, normal
            v = -r.dir;
            n = trace.hitnorm;
            if (trace.hitobj != nullptr) {
                if (glm::dot(trace.hitnorm, r.dir) > 0) {
                    std::cout << "Back facing trace" << std::endl;
                }
                // TODO Shading and materials
                if (opts.debug_flags & debug_mode::normal_coloring) {
                    color = (vec3(trace.hitnorm) + vec3(1.0, 1.0, 1.0)) * (scalar)0.5;
                } else if (opts.debug_flags & debug_mode::interp_coloring) {
                    color = vec3(trace.barycenter);
                } else {
                    for (auto& baselight : m_lights) {
                        vec4 l, h; // Light dir, half dir
                        vec3 El; // Irradiance
                        switch (baselight->type()) {
                            case LightType::Directional: {
                                auto* light = dynamic_cast<DirectionalLight*>(baselight.get());
                                El = light->color() * light->intensity();
                                l = -light->direction();
                            } break;
                            case LightType::Point: {
                                auto* light = dynamic_cast<PointLight*>(baselight.get());
                                l = light->position() - trace.hitpos;
                                scalar r2 = glm::dot(l, l);
                                l = glm::normalize(l);
                                El = light->color() * light->intensity() / r2;
                            } break;
                        }
                        h = glm::normalize(v + l);
                        color += glm::one_over_pi<scalar>() * vec3(1.0, 1.0, 1.0) * El * glm::dot(l,n);
                    }
                }
            }
    }
    return color;
}
