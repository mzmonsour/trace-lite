#pragma once

#include "model.h"
#include "light.h"
#include "trace.h"

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

    public:

        /**
         * Construct a camera given a transform, vertical fov, and aspect ratio.
         *
         * @param xform Camera transform.
         * @param fov Vertical fov in radians.
         * @param aspect Aspect ratio given as width / height.
         */
        Camera(mat4 xform, scalar fov, scalar aspect);

        ~Camera() {}

        mat4 get_transform() const { return m_xform; }

        /**
         * Set the camera transform.
         */
        void set_transform(mat4 xform) { m_xform = glm::inverse(xform); }

        /**
         * Compute a ray directed at a virtual screen.
         *
         * @param pos Position of the ray on the screen from [-1,1]. (-1, -1) lies at the top left, and
         * (1, 1) lies at the bottom right.
         */
        ray compute_ray(vec2 pos) const;
};

class Scene {
    private:

        const std::vector<Model>&                   m_objects;
        const std::vector<std::unique_ptr<Light>>   m_lights;

    public:

        Scene(const std::vector<Model>& objects, std::vector<std::unique_ptr<Light>> lights);

        ~Scene() {}

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
        vec3 compute_ray_color(const ray& r, const render_options& opts, size_t steps) const;
};
