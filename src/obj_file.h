#pragma once

#include "model.h"

/**
 * Wavefront OBJ file loader.
 */
class ObjFile {
    private:

        std::string         m_path;
        std::vector<Model>  m_objects;

    public:

        /**
         * Loads the OBJ file at the given path.
         */
        ObjFile(const std::string& path);

        ~ObjFile() {}

        /**
         * Get the models described by the file.
         */
        const std::vector<Model>& get_models() const { return m_objects; }

};
