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

#include <iostream>
#include <iomanip>
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
    std::string cam_name;
    int img_width, img_height;
    scalar fov;
    size_t threads;

    int result = 0;
    bool show_help = false;

    po::options_description opts("Render options");
    po::positional_options_description posopts;
    posopts.add("input", 1);
    opts.add_options()
        ("help", "Display help")
        ("input", "Input file location")
        ("output,o", po::value<std::string>(&outfile)->default_value("render.png"), "Output file location (defaults to render.png)")
        ("width,w", po::value<int>(&img_width)->default_value(1920), "Width of the output image")
        ("height,h", po::value<int>(&img_height)->default_value(1080), "Height of the output image")
        ("no-aspect-override", "Do not override the camera aspect with the render resolution")
        ("camera,c", po::value<std::string>(&cam_name), "Name of the camera to use for rendering")
        ("normal-coloring", "Enable normal coloring mode")
        ("interp-coloring", "Enable interpolated coloring mode")
        ("fov", po::value<scalar>(&fov), "Override camera field of view. Given as vertical FOV in degrees.")
        ("msaa", "Enable Multisample Anti-Aliasing")
        ("threads,t", po::value<size_t>(&threads)->default_value(0), "Number of rendering threads (0 uses a reasonable default)")
        ;
    po::variables_map argmap;
    try {
        po::store(po::command_line_parser(argc, argv).options(opts).positional(posopts).run(), argmap);
        po::notify(argmap);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        result = 1;
        show_help = true;
    }

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
        std::cout << "Usage: " << argv[0] << " [options] input.obj" << std::endl << std::endl;
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

    std::vector<std::unique_ptr<Light>> lights;
    std::unique_ptr<Light> dlight = std::make_unique<DirectionalLight>(
                vec3(1.0, 1.0, 1.0), 1.0,
                glm::normalize(vec4(1.0, -1.0, 0.0, 0.0))
                );
    lights.push_back(std::move(dlight));

    Camera cam;
    if (scene_graph.assimp_scene()->mNumCameras > 0) {
        auto *s = scene_graph.assimp_scene();
        aiCamera *assimp_cam = s->mCameras[0];
        if (argmap.count("camera")) {
            aiString target_name(cam_name);
            for (size_t i = 0; i < s->mNumCameras; ++i) {
                if (s->mCameras[i]->mName == target_name) {
                    assimp_cam = s->mCameras[i];
                    break;
                }
            }
        }
        std::cout << "Using camera " << std::quoted(assimp_cam->mName.C_Str()) << std::endl;
        cam = Camera(*scene_graph.assimp_scene(), *assimp_cam);
        if (!argmap.count("no-aspect-override")) {
            cam.set_aspect(((scalar)img_width) / ((scalar)img_height));
        }
        if (argmap.count("fov")) {
            cam.set_fov(glm::radians(fov));
        }
    } else {
        std::cout << "No cameras imported; Falling back to default" << std::endl;
    }

    render_options ropts;
    ropts.width = img_width;
    ropts.height = img_height;
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

