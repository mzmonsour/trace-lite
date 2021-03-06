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

#include "model.h"
#include "light.h"
#include "trace.h"
#include "scene.h"
#include "bvh.h"

#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>
#include <cstdint>

/**
 * Perform gamma correction, moving from linear colors to sRGB.
 */
vec3 linear_to_srgb(vec3 color);

/**
 */
namespace debug_mode {
    const static int none               = 0; // No debug modes enabled
    const static int normal_coloring    = 1 << 0; // Color surfaces by their normals
    const static int interp_coloring    = 1 << 1; // Color surfaces by their barycentric coordinates
};

struct render_options {

    /**
     * Construct with default render options.
     */
    render_options();

    uint16_t width, height;
    int debug_flags; // Select bitflags from debug_mode
    bool msaa; // Enable MSAA
    size_t max_recursion; // Maximum number of recursive steps in renderer
    size_t concurrency; // Number of concurrent rendering jobs
};

struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class Camera {
    private:

        mat4 m_xform;
        scalar m_fov, m_aspect;
        bool m_is_fov_horizontal;

    public:

        /**
         * Construct a default camera.
         */
        Camera();

        /**
         * Construct a camera given a transform, vertical fov, and aspect ratio.
         *
         * @param xform Camera transform.
         * @param fov Vertical fov in radians.
         * @param aspect Aspect ratio given as width / height.
         */
        Camera(mat4 xform, scalar fov, scalar aspect);

        /**
         * Construct a camera from an assimp camera in the scene graph.
         *
         * @param scene Scene graph containing the camera.
         * @param camera Assimp camera object
         */
        Camera(const aiScene& scene, const aiCamera& camera);

        ~Camera() {}

        mat4 get_transform() const { return m_xform; }

        /**
         * Set the camera transform.
         */
        void set_transform(mat4 xform) { m_xform = glm::inverse(xform); }

        /**
         * Set the vertical FOV
         */
        void set_fov(scalar fov);

        /**
         * Set the aspect ratio
         *
         * @param aspect New aspect ratio
         * @param use_vertical_fov  If true, setting the aspect will always keep the vertical FOV.
         *                          Otherwise, it will keep whatever FOV was originally set.
         */
        void set_aspect(scalar aspect, bool keep_vertical_fov=false);

        /**
         * Compute a ray directed at a virtual screen.
         *
         * @param pos Position of the ray on the screen from [-1,1]. (-1, -1) lies at the top left, and
         * (1, 1) lies at the bottom right.
         */
        Ray compute_ray(vec2 pos) const;
};

class Renderer {
    private:

        const Scene& m_scene;
        BVH m_bvh;
        const std::vector<std::unique_ptr<Light>> m_lights;

    public:

        Renderer(const Scene& scene_graph, std::vector<std::unique_ptr<Light>> lights);

        ~Renderer() {}

        /**
         * Render the scene using a recursive ray-tracing method.
         *
         * @param cam Camera from which to render the scene.
         * @param opts Additional options for the renderer, such as resolution.
         * @return Raw RGB image data.
         */
        std::vector<rgb_color> render(Camera& cam, render_options opts) const;

        /**
         * Render a subset of the scene using recursive ray-tracing.
         *
         * @param data Output RGB image data. Data is formatted using range width/height, no blank
         * space is left for the final image.
         * @param cam Camera from which to render the scene.
         * @param opts Additional options for the renderer.
         * @param x Starting x position of the range.
         * @param y Starting y position of the range.
         * @param width Width of the range. x + width must not exceed the final render width.
         * @param height Height of the range. y + height must not exceed the final render height.
         */
        void render_range(  std::vector<rgb_color>& data,
                            const Camera& cam,
                            const render_options& opts,
                            uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height) const;

        /**
         * Compute the color of a ray of light traveling through the scene.
         *
         * @param ray Ray opposing the ray of light in question.
         * @param opts Options for the renderer, which may affect lighting computation.
         * @param steps Number of recursive steps taken to compute reflections.
         */
        vec3 compute_ray_color(const Ray& r, const render_options& opts, size_t steps) const;
};
