#pragma once

#include "model.h"
#include "trace.h"

#include <glm/mat4x4.hpp>
#include <vector>
#include <cstdint>

/**
 * Perform gamma correction, moving from linear colors to sRGB.
 */
glm::vec3 linear_to_srgb(glm::vec3 color);

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
};

struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class Camera {
    private:

        glm::mat4x4 m_xform;
        float m_fov, m_aspect;

    public:

        /**
         * Construct a camera given a transform, vertical fov, and aspect ratio.
         *
         * @param xform Camera transform.
         * @param fov Vertical fov in radians.
         * @param aspect Aspect ratio given as width / height.
         */
        Camera(glm::mat4x4 xform, float fov, float aspect);

        ~Camera() {}

        glm::mat4x4 get_transform() const { return m_xform; }

        /**
         * Set the camera transform.
         */
        void set_transform(glm::mat4x4 xform) { m_xform = glm::inverse(xform); }

        /**
         * Compute a ray directed at a virtual screen.
         *
         * @param pos Position of the ray on the screen from [-1,1]. (-1, -1) lies at the top left, and
         * (1, 1) lies at the bottom right.
         */
        ray compute_ray(glm::vec2 pos) const;
};

class Scene {
    private:

        const std::vector<Model>& m_objects;

    public:

        Scene(const std::vector<Model>& objects);

        ~Scene() {}

        /**
         * Render the scene using a recursive ray-tracing method.
         *
         * @param cam Camera from which to render the scene.
         * @param opts Additional options for the renderer, such as resolution.
         * @return Raw RGB image data.
         */
        std::vector<rgb_color> render(Camera& cam, render_options opts) const;
};
