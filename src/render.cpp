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

#include "render.h"
#include "convert.h"
#include "const.h"
#include "assimp_tools.h"
#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <thread>
#include <functional>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

render_options::render_options() :
    width(640),
    height(480),
    debug_flags(debug_mode::none),
    msaa(false),
    max_recursion(1),
    concurrency(1)
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

Camera::Camera() :
    m_xform(1.0),
    m_fov(glm::radians(90.0)),
    m_aspect(16.0 / 9.0) {}

Camera::Camera(mat4 xform, scalar fov, scalar aspect) :
    m_xform(glm::inverse(xform)),
    m_fov(fov),
    m_aspect(aspect),
    m_is_fov_horizontal(false)
{
}

Camera::Camera(const aiScene& scene, const aiCamera& camera) :
    m_fov(camera.mHorizontalFOV / camera.mAspect),
    m_aspect(camera.mAspect),
    m_is_fov_horizontal(true)
{
    mat4 node_xform = MAT4_IDENTITY;
    std::cout << "Camera fov: " << m_fov << std::endl;
    std::cout << "Camera aspect: " << m_aspect << std::endl;
    if (search_assimp_scene_graph(scene, camera.mName, node_xform) != nullptr) {
        auto pos = vec3(node_xform * assimp_vec_to_glm4(camera.mPosition, 1.0));
        auto look_at = vec3(node_xform * assimp_vec_to_glm4(camera.mLookAt, 0.0));
        auto up = vec3(node_xform * assimp_vec_to_glm4(camera.mUp, 0.0));
        m_xform = glm::inverse(glm::lookAt(pos, look_at, up));
    } else {
        m_xform = MAT4_IDENTITY;
    }
}

void Camera::set_fov(scalar fov)
{
    m_fov = fov;
    m_is_fov_horizontal = false;
}

void Camera::set_aspect(scalar aspect, bool keep_vertical_fov)
{
    if (!keep_vertical_fov && m_is_fov_horizontal) {
        scalar h_fov = m_fov * m_aspect;
        m_fov = h_fov / aspect;
    }
    m_aspect = aspect;
}

Ray Camera::compute_ray(vec2 pos) const
{
    Ray out;
    scalar x, y;
    scalar tan = glm::tan(m_fov * 0.5);
    x = tan * m_aspect * pos.x;
    y = tan * pos.y;
    out.dir = glm::normalize(m_xform * vec4(x, y, -1.0, 0.0));
    out.origin = m_xform * vec4(0.0, 0.0, 0.0, 1.0); // TODO Cache origin?
    return out;
}

Renderer::Renderer(const Scene& scene_graph, std::vector<std::unique_ptr<Light>> lights) :
    m_scene(scene_graph),
    m_bvh(scene_graph),
    m_lights(std::move(lights))
{
}

/**
 * Render a range of pixels in the final image.
 */
void Renderer::render_range(   std::vector<rgb_color>& data,
                            const Camera& cam,
                            const render_options& opts,
                            uint16_t initx, uint16_t inity,
                            uint16_t width, uint16_t height) const
{
    size_t progress = 0, percent = 0;
    for (int y = inity; y < inity + height; ++y) {
        for (int x = initx; x < initx + width; ++x) {
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
                Ray view_ray = cam.compute_ray(
                        vec2(  2.0 * (((scalar)x*msfactor) + sx) / ((scalar)opts.width * msfactor) - 1.0,
                                    1.0 - 2.0 * (((scalar)y*msfactor) + sy) / ((scalar)opts.height * msfactor)));
                vec3 sample = this->compute_ray_color(view_ray, opts, opts.max_recursion);
                samples.push_back(glm::clamp(sample, (scalar)0.0, (scalar)1.0));
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
            data.push_back(imgcolor);
            progress++;
        }
        if ((progress * 100 / opts.concurrency) / (width * height) > percent) {
            percent++;
            std::cout << "." << std::flush;
        }
    }
}

std::vector<rgb_color> Renderer::render(Camera& cam, render_options opts) const
{
    std::vector<rgb_color> img;
    img.reserve(opts.width * opts.height);
    std::cout << "Rendering..." << std::flush;
    std::vector<std::vector<rgb_color>> thread_data;
    std::vector<std::thread> thread_handles;
    uint16_t y, height;
    y = 0;
    height = opts.height / opts.concurrency;
    thread_data.resize(opts.concurrency);
    for (size_t t = 0; t < opts.concurrency; t++) {
        if (t == opts.concurrency - 1) {
            height = opts.height - y;
        }
        thread_data[t].reserve(opts.width * height);
        thread_handles.emplace_back(&Renderer::render_range, this,
                std::ref(thread_data[t]),
                std::cref(cam),
                std::cref(opts),
                0, y, opts.width, height);
        y += height;
    }
    for (auto& h : thread_handles) {
        h.join();
    }
    for (auto& data : thread_data) {
        img.insert(img.end(), data.cbegin(), data.cend());
    }
    std::cout << "done!" << std::endl;
    return img;
}

vec3 Renderer::compute_ray_color(const Ray& r, const render_options& opts, size_t steps) const
{
    vec3 color(0.0, 0.0, 0.0);
    if (steps == 0) {
        return color;
    }
    trace_info trace = m_bvh.trace_ray(r);
    if (trace.intersect_type == IntersectionType::Intersected) {
        vec4 v, n; // View dir, normal
        v = -r.dir;
        n = trace.hitnorm;
#ifdef BACKFACE_DIAGNOSTIC
                if (glm::dot(trace.hitnorm, r.dir) > 0) {
                    std::cout << "Back facing trace" << std::endl;
                }
#endif
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
    return color;
}
