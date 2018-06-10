#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/program_options.hpp>
#include "obj_file.h"
#include "model.h"
#include "render.h"
#include "png_helper.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

int main(int argc, char **argv)
{
    namespace po = boost::program_options;

    std::string outfile;
    int img_width, img_height;

    po::options_description opts("Hello");
    po::positional_options_description posopts;
    posopts.add("input", 1);
    opts.add_options()
        ("help", "Display help")
        ("input", "Input file location")
        ("output,o", po::value<std::string>(&outfile)->default_value("render.png"), "Output file location (defaults to render.png)")
        ("width,w", po::value<int>(&img_width)->default_value(1920), "Width of the output image")
        ("height,h", po::value<int>(&img_height)->default_value(1080), "Height of the output image");
    po::variables_map argmap;
    po::store(po::command_line_parser(argc, argv).options(opts).positional(posopts).run(), argmap);
    po::notify(argmap);

    int result = 0;
    bool show_help = false;

    if (argmap.count("help")) {
        show_help = true;
        result = 0;
    }

    if (!argmap.count("input") && !show_help) {
        std::cerr << "Must provide input file" << std::endl;
        show_help = true;
        result = 1;
    }

    if (show_help) {
        std::cout << "Usage: " << argv[0] << " [options] input.obj" << std::endl;
        std::cout << opts;
        return result;
    }

    std::string infile = argmap["input"].as<std::string>();
    std::cout << "Loading OBJ file \"" << infile << "\"" << std::endl;
    ObjFile obj = ObjFile(infile);
    std::cout << obj.get_models().size() << " model(s) found:" << std::endl;
    for (auto& mdl : obj.get_models()) {
        std::cout << "Model \"" << mdl.get_name() << "\" ("
            << mdl.get_vertices().size() << " vertices)" << std::endl;
    }

    render_options ropts;
    ropts.width = img_width;
    ropts.height = img_height;
    Camera cam(glm::lookAt(glm::vec3(0.0, 10.0, 10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)),
            3.141592 / 2.0, 16.0/9.0);
    Scene scene(obj.get_models());
    std::vector<rgb_color> imgdata = scene.render(cam, ropts);


    pnghelper_write_image_file(outfile.c_str(), &imgdata[0], img_width, img_height);

    return result;
}

