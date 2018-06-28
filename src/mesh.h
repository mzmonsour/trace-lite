#pragma once

#include "types.h"
#include "aabb.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/mesh.h>
#include <vector>
#include <array>

/**
 * A 3D mesh composed of triangles.
 */
class Mesh {
    public:

        struct face;

    private:

        std::string m_name;
        std::vector<face> m_faces;
        std::vector<vec4> m_vertices;
        std::vector<vec4> m_plane_normals;
        std::vector<vec4> m_normals;
        std::vector<vec2> m_uvs;
        aabb m_aabb;

    public:

        struct face {
            size_t index[3];
        };

        class TriangleIter;

        class TriangleContainer {
            private:

                const Mesh& m_mesh;

            public:

                TriangleContainer(const Mesh& mesh) : m_mesh(mesh) {}

                /**
                 * Get beginning iterator over triangles in the mesh.
                 */
                inline TriangleIter begin() const;

                /**
                 * Get ending iterator over triangles in the mesh.
                 */
                inline TriangleIter end() const;
        };

        class Triangle {
            private:

                const Mesh *m_mesh;
                size_t m_face;

                friend class Mesh::TriangleIter;

            public:

                Triangle(const Mesh *mesh, size_t i) :
                    m_mesh(mesh), m_face(i) {}

                /**
                 * Get the first vertex of the triangle.
                 */
                inline const vec4& p0() const;

                /**
                 * Get the second vertex of the triangle.
                 */
                inline const vec4& p1() const;

                /**
                 * Get the third vertex of the triangle.
                 */
                inline const vec4& p2() const;

                /**
                 * Get the normal of the plane given by the points of the triangle.
                 */
                inline const vec4& plane_normal() const;

                /**
                 * Compute the normal at a point on the surface of the triangle.
                 *
                 * @param coords Barycentric coordinates of the point on the surface.
                 */
                inline vec4 surface_normal(vec3 coords) const;

                /**
                 * Compute the UV coordinates at a point on the surface of the triangle.
                 *
                 * @param coords Barycentric coordinates of the point on the surface.
                 */
                inline vec2 surface_uvs(vec3 coords) const;

                inline bool operator==(const Triangle& b) const;
                inline bool operator!=(const Triangle& b) const;
        };

        /**
         * Forward iterator over the triangles of a mesh.
         */
        class TriangleIter {
            private:

                Triangle m_face;

            public:

                TriangleIter() : m_face(nullptr, 0) {}
                TriangleIter(const Mesh *mesh, size_t first) : m_face(mesh, first) {}

                inline TriangleIter& operator++();
                inline TriangleIter operator++(int);

                inline bool operator==(const TriangleIter& b) const;
                inline bool operator!=(const TriangleIter& b) const;

                inline Triangle& operator*();
                inline Triangle * operator->();
        };

        Mesh();

        /**
         * Attempt to construct a mesh from an assimp aiMesh. Automatically converts coordinates to
         * glm equivalents. Also ensures the mesh is a valid format.
         *
         * @throws invalid_argument Thrown if the mesh is not fully triangulated.
         */
        Mesh(const aiMesh& mesh);

        /**
         * Get the name of the mesh.
         */
        const std::string& name() const { return m_name; }

        /**
         * Get the list of faces.
         */
        const std::vector<face>& faces() const { return m_faces; }

        /**
         * Get the list of vertices.
         */
        const std::vector<vec4>& vertices() const { return m_vertices; }

        /**
         * Get the list of plane normals. Indices correspond to the faces.
         */
        const std::vector<vec4>& plane_normals() const { return m_plane_normals; }

        /**
         * Get the list of normals. Indices correspond to the vertices.
         */
        const std::vector<vec4>& normals() const { return m_normals; }

        /**
         * Get the list of UV coordinates. Indices correspond to the vertices.
         */
        const std::vector<vec2>& uv_coordinates() const { return m_uvs; }

        /**
         * Get the list of triangles.
         */
        const TriangleContainer triangles() const { return TriangleContainer(*this); }

        /**
         * Get the object space AABB for this mesh.
         */
        const aabb& object_space_aabb() const { return m_aabb; }
};

/**
 * An instance of a mesh in the scene.
 */
class MeshInstance {
    private:

        const Mesh& m_mesh;
        mat4 m_xform;
        mat4 m_inv_xform;

    public:


        /**
         * Construct a mesh instance at a given transform.
         */
        MeshInstance(const Mesh& mesh, mat4 xform);

        /**
         * Get the transform from object to world space.
         */
        const mat4& transform() const { return m_xform; }

        /**
         * Get the transform from world to object space.
         */
        const mat4& inverse_transform() const { return m_inv_xform; }

        /**
         * Get the mesh associated with this instance.
         */
        const Mesh& mesh() const { return m_mesh; }
};

#include "mesh.inl"
