#include "mesh_instance.h"

MeshInstance::MeshInstance(aiMesh* mesh, mat4 xform) :
    m_mesh(mesh),
    m_xform(xform),
    m_inv_xform(glm::inverse(xform)) {}
