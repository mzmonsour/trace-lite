#pragma once

#include "types.h"

#include <glm/vec4.hpp>
#include <vector>
#include <string>

struct FaceIndex {
    int vertices[3];
    int normals[3];
    int tri_normal;
};

class Model {
    private:

        const std::string m_name;

        size_t m_missing_norm_idx;
        std::vector<vec4>  m_vertices;
        std::vector<vec4>  m_normals;
        std::vector<FaceIndex>  m_triangles;

    public:

        class triangle_iter;

        class triangle {
            private:

                const Model& m_container;
                const FaceIndex* m_idx;

                friend class Model::triangle_iter;

            public:

                triangle(const Model& m);
                ~triangle() {}

                /**
                 * Get the first vertex of the triangle.
                 */
                vec4 p0() const;

                /**
                 * Get the second vertex of the triangle.
                 */
                vec4 p1() const;

                /**
                 * Get the third vertex of the triangle.
                 */
                vec4 p2() const;

                /**
                 * Get the face normal of the triangle. This is different that the surface normal,
                 * which may be interpolated or given by a normal map.
                 */
                vec4 face_normal() const;
                
                /**
                 * Compute the normal at a point on the surface of the triangle.
                 *
                 * @param coords Barycentric coordinates of the point on the surface.
                 */
                vec4 surface_normal(vec3 coords) const;

                /**
                 * Compute the UV coordinates at a point on the surface of the triangle.
                 *
                 * @param coords Barycentric coordinates of the point on the surface.
                 */
                vec2 surface_uvs(vec3 coords) const;
        };

        class triangle_iter {
            private:

                const Model& m_container;
                std::vector<FaceIndex>::const_iterator m_idx_iter;
                triangle m_current;

            public:

                triangle_iter(const Model& m, std::vector<FaceIndex>::const_iterator iter);
                ~triangle_iter() {}

                triangle_iter& operator++();
                triangle_iter operator++(int);

                bool operator==(triangle_iter& b) const;
                bool operator!=(triangle_iter& b) const;

                triangle& operator*();
                triangle * operator->();

        };

        /**
         * Create a model from a list of vertices, normals, and triangles.
         */
        Model(  const std::string name,
                std::vector<vec4> vertices,
                std::vector<vec4> normals,
                std::vector<FaceIndex> triangles);

        Model() {}

        ~Model() {}

        /**
         * Get the name of the model.
         */
        const std::string& get_name() const { return m_name; }

        /**
         * Get the model's vertices.
         */
        const std::vector<vec4>& get_vertices() const { return m_vertices; }

        /**
         * Get the model's normals.
         */
        const std::vector<vec4>& get_normals() const { return m_normals; }

        /**
         * Get the model's face indices.
         */
        const std::vector<FaceIndex>& get_face_indices() const { return m_triangles; }

        /**
         * Beginning iterator over all triangles in the model.
         */
        triangle_iter begin() const;

        /**
         * Ending iterator over all triangles in the model.
         */
        triangle_iter end() const;
};
