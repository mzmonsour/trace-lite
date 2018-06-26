#pragma once

#include "types.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/mesh.h>

/**
 * An instance of a mesh in the scene.
 */
class MeshInstance {
    private:

        aiMesh* m_mesh;
        mat4 m_xform;
        mat4 m_inv_xform;

    public:

        /**
         * Construct a mesh instance at a given transform.
         */
        MeshInstance(aiMesh* mesh, mat4 xform);

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
        const aiMesh* mesh() const { return m_mesh; }

};
