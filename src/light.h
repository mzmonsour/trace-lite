#pragma once

#include <types.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum struct LightType {
    Directional,
    Point,
};

class Light {
    public:

        virtual ~Light() {};

        virtual LightType type() const = 0;
};

class DirectionalLight : public Light {
    private:

        vec3    m_color;
        scalar  m_intensity;
        vec4    m_dir;

    public:

        DirectionalLight(vec3 color, scalar intensity, vec4 dir) : m_color(color), m_intensity(intensity), m_dir(dir) {}

        vec3 color() const { return m_color; }

        scalar intensity() const { return m_intensity; }

        vec4 direction() const { return m_dir; }

        virtual LightType type() const { return LightType::Directional; }
};

class PointLight : public Light {
    private:

        vec3    m_color;
        scalar  m_intensity;
        vec4    m_pos;

    public:

        PointLight(vec3 color, vec4 pos, vec4 dir) : m_color(color), m_pos(pos) {}

        vec3 color() const { return m_color; }

        scalar intensity() const { return m_intensity; }

        vec4 position() const { return m_pos; }

        virtual LightType type() const { return LightType::Point; }
};
