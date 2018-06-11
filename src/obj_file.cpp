#include "model.h"
#include "obj_file.h"
#include <glm/vec4.hpp>
#include <stdexcept>
#include <sstream>
#include <fstream>

enum struct ParseFaceResult {
    Ok,
    BadIndex,
    NotEnoughVertices,
    TooManyVertices,
};

ParseFaceResult parse_obj_face(std::istringstream& in, FaceIndex& out)
{
    int vertices = 0;
    std::string indexgroup;
    while (in >> indexgroup) {
        if (vertices >= 3) {
            return ParseFaceResult::TooManyVertices;
        }
        size_t delimA, delimB;
        delimA = indexgroup.find('/');
        delimB = indexgroup.find('/', delimA + 1);
        try {
            // TODO: Handle inconsistent number of indices. e.g. If one vertex has normals, all 3 should.
            if (delimA == std::string::npos) {
                // Vertex only
                out.vertices[vertices] = std::stof(indexgroup);
                // TODO: -1 is a poor sentinel value. Prevents negative indexing on normals.
                out.normals[vertices] = -1;
            } else if (delimB == std::string::npos) {
                // Vertex and texcoords
                out.vertices[vertices] = std::stof(indexgroup.substr(0, delimA));
                out.normals[vertices] = -1;
            } else {
                // Vertex, texcoords, and normals
                out.vertices[vertices] = std::stof(indexgroup.substr(0, delimA));
                out.normals[vertices] = std::stof(indexgroup.substr(delimB+1));
            }
        } catch (std::invalid_argument& ex) {
            return ParseFaceResult::BadIndex;
        }
        vertices++;
    }
    if (vertices < 3) {
        return ParseFaceResult::NotEnoughVertices;
    } else {
        return ParseFaceResult::Ok;
    }
}

ObjFile::ObjFile(const std::string& path) :
    m_path(path)
{
    std::ifstream file(path);
    std::string line;
    if (file) {
        std::string objname = "(anonymous)";
        std::vector<glm::vec4> vertices, normals;
        std::vector<FaceIndex> triangles;
        while (std::getline(file, line)) {
            if (line.size() == 0 || line.front() == '#') {
                continue;
            }
            std::istringstream lparse(line);
            std::string keyword;
            if (!(lparse >> keyword)) {
                // Skip errors for now
                continue;
            }
            if (keyword == "o") {
                if (vertices.size() > 0) {
                    m_objects.emplace_back(objname, std::move(vertices),
                            std::move(normals), std::move(triangles));
                    vertices.clear();
                    normals.clear();
                    triangles.clear();
                }
                lparse >> objname;
            } else if (keyword == "v") {
                float x,y,z;
                if (lparse >> x >> y >> z) {
                    vertices.push_back(glm::vec4(x, y, z, 1.0));
                }
            } else if (keyword == "vn") {
                float x,y,z;
                if (lparse >> x >> y >> z) {
                    normals.push_back(glm::vec4(x, y, z, 0.0));
                }
            } else if (keyword == "f") {
                FaceIndex fidx;
                switch (parse_obj_face(lparse, fidx)) {
                    case ParseFaceResult::Ok: {
                        for (int i = 0; i < 3; ++i) {
                            if (fidx.vertices[i] < 0) {
                                fidx.vertices[i] += vertices.size();
                            }
                            // TODO: Negative indexing on normals. (see parser also)
                        }
                        triangles.push_back(fidx);
                    } break;
                    default: {
                        // TODO: Log parse error
                    } break;
                }
            }
        }
        if (vertices.size() > 0) {
            m_objects.emplace_back(objname, std::move(vertices),
                    std::move(normals), std::move(triangles));
        }
    } else {
        // TODO: Report error
    }
}

void dump_obj_file(std::string& path, const Model& m)
{
    std::ofstream file(path);
    auto& indices = m.get_face_indices();
    auto& vertices = m.get_vertices();
    auto& normals = m.get_normals();
    if (file) {
        file << "# BEGIN DUMP" << std::endl;
        file << "o " << m.get_name() << std::endl;
        for (auto& v : vertices) {
            file << "v " << v.x << ' ' << v.y << ' ' << v.z << std::endl;
        }
        for (auto& n : normals) {
            file << "vn " << n.x << ' ' << n.y << ' ' << n.z << std::endl;
        }
        for (auto& idx : indices) {
            file << "f "
                << idx.vertices[0] << "//" << idx.normals[0] << ' '
                << idx.vertices[1] << "//" << idx.normals[1] << ' '
                << idx.vertices[2] << "//" << idx.normals[2] << std::endl;
        }
        file << "# END DUMP" << std::endl;
    }
}
