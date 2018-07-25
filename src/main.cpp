#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/program_options.hpp>
#include <thread>
#include "obj_file.h"
#include "model.h"
#include "light.h"
#include "render.h"
#include "png_helper.h"
#include "types.h"
#include "assimp_tools.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "scene.h"

int main(int argc, char **argv)
{
    namespace po = boost::program_options;

    std::string outfile, dumpfile;
    int img_width, img_height;
    vec3 eyepos;
    std::string eyestr;
    scalar fov;
    size_t threads;

    po::options_description opts("Hello");
    po::positional_options_description posopts;
    posopts.add("input", 1);
    opts.add_options()
        ("help", "Display help")
        ("input", "Input file location")
        ("output,o", po::value<std::string>(&outfile)->default_value("render.png"), "Output file location (defaults to render.png)")
        ("width,w", po::value<int>(&img_width)->default_value(1920), "Width of the output image")
        ("height,h", po::value<int>(&img_height)->default_value(1080), "Height of the output image")
        ("normal-coloring", "Enable normal coloring mode")
        ("interp-coloring", "Enable interpolated coloring mode")
        ("eye", po::value<std::string>(&eyestr)->default_value("0,0,10"), "Eye position of camera")
        ("fov,f", po::value<scalar>(&fov)->default_value(90.0f), "Vertical field of view in degrees")
        ("msaa", "Enable Multisample Anti-Aliasing")
        ("threads,t", po::value<size_t>(&threads)->default_value(0), "Number of rendering threads (0 uses a reasonable default)")
        ;
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
    std::cout << "Loading scene from file \"" << infile << "\"" << std::endl;
    Scene scene_graph(infile);
    {
        auto *s = scene_graph.assimp_scene();
        if (s != nullptr && s->mNumMeshes > 0) {
            std::cout << "Loaded " << s->mNumMeshes << " meshes" << std::endl;
            for (size_t i = 0; i < s->mNumMeshes; ++i) {
                auto *mesh = s->mMeshes[i];
                std::cout << "\tMesh[" << i << "]: " << mesh->mName.C_Str()
                    << " (" << mesh->mNumVertices << " vertices)" << std::endl;
            }
            log_assimp_scene_graph(*s);
        } else {
            std::cout << "Assimp error: No meshes loaded" << std::endl;
            return 1;
        }
    }

    std::string eyecomp;
    std::istringstream eyeparse(eyestr);
    for (int i = 0; i < 3; ++i) {
        std::getline(eyeparse, eyecomp, ',');
        eyepos[i] = std::stof(eyecomp);
    }

    std::vector<std::unique_ptr<Light>> lights;
    std::unique_ptr<Light> dlight = std::make_unique<DirectionalLight>(
                vec3(1.0, 1.0, 1.0), 1.0,
                glm::normalize(vec4(1.0, -1.0, 0.0, 0.0))
                );
    lights.push_back(std::move(dlight));

    render_options ropts;
    ropts.width = img_width;
    ropts.height = img_height;
    Camera cam(glm::lookAt(eyepos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0)),
            glm::radians(fov), ((scalar)img_width)/((scalar)img_height));
    Renderer renderer(scene_graph, std::move(lights));
    ropts.debug_flags = debug_mode::none;
    if (argmap.count("normal-coloring")) {
        std::cout << "DEBUG: Normal coloring mode enabled" << std::endl;
        ropts.debug_flags |= debug_mode::normal_coloring;
    }
    if (argmap.count("interp-coloring")) {
        std::cout << "DEBUG: Interpolated coloring mode enabled" << std::endl;
        ropts.debug_flags |= debug_mode::interp_coloring;
    }
    if (argmap.count("msaa")) {
        ropts.msaa = true;
    }
    if (threads == 0) {
        threads = std::thread::hardware_concurrency();
    }
    ropts.concurrency = threads;
    std::cout << "Using " << threads << " rendering threads" << std::endl;
    std::vector<rgb_color> imgdata = renderer.render(cam, ropts);

    pnghelper_write_image_file(outfile.c_str(), &imgdata[0], img_width, img_height);

    return result;
}

