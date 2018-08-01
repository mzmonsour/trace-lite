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
