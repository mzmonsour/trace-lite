#include <iostream>
#include <glm/vec3.hpp>
#include "obj_file.h"
#include "model.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        return 1;
    }
    std::cout << "Loading OBJ file \"" << argv[1] << "\"" << std::endl;
    ObjFile obj = ObjFile(std::string(argv[1]));
    std::cout << obj.get_models().size() << " model(s) found:" << std::endl;
    for (auto& mdl : obj.get_models()) {
        std::cout << "Model \"" << mdl.get_name() << "\" ("
            << mdl.get_vertices().size() << " vertices)" << std::endl;
    }
    return 0;
}

