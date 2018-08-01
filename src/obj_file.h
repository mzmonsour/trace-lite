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

#include "types.h"
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

/**
 * Dumps a Model to an object file, for debugging purposes.
 */
void dump_obj_file(std::string& path, const Model& m);
