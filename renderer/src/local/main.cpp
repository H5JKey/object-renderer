#include <cstdlib>
#include <exception>

#include "logger.hpp"
#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"

void printHelp(std::string_view programName) {
    std::println("{} - offline path tracer\n", programName);
    std::println("Usage:");
    std::println(" {} <width> <height> <samples> <input_scene> [OPTIONS]\n", programName);
    std::println("Arguments:");
    std::println("  width:        Output image width");
    std::println("  height:       Output image height");
    std::println("  samples:      Path tracer samples");
    std::println("  input_model:  Path to 3D scene file (only .glb, .gltf supported)");
    std::println("Options:");
    std::println("-h, --help      Shows this help message");
    std::println("-o, --output    Output image path (default: output.png)");
    std::println("-v, --verbose   Print detailed logs");
    std::println("-d, --debug     Output debug images: raw, albedo, normals");
}

void printUsage(std::string_view programName) {
    std::println(std::cerr, "Usage: {} <width> <height> <samples> <input_scene> [OPTIONS]", programName);
    std::println(std::cerr, "Try {} --help for more information", programName);
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return EXIT_SUCCESS;
        }
    }
    if (argc < 5) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    int width, height, samples;
    std::string input;
    std::string output = "output.png";
    bool debugImages = false;
    try {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
        samples = std::stoi(argv[3]);
    } catch (const std::exception& e) {
        std::println(std::cerr, "Error: invalid number format");
        std::println(std::cerr, "  width, height, and samples must be integers");
        printUsage(argv[0]);
        return 1;
    }
    input = argv[4];
    for (int i = 5; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return EXIT_SUCCESS;
        } else if (arg == "-v" || arg == "--verbose")
            Logger::getInstance().debug = true;
        else if (arg == "-d" || arg == "--debug")
            debugImages = true;
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 == argc) {
                std::println(std::cerr, "Error: -o requires an argument");
                return EXIT_FAILURE;
            }
            output = argv[++i];
            size_t dot = output.find_last_of('.');
            if (dot != std::string::npos) {
                output = output.substr(0, dot);
            }
        } else {
            std::println(std::cerr, "Unrecognized option: {}", argv[i]);
            std::println(std::cerr, "Try {} --help for more information", argv[0]);
            return EXIT_FAILURE;
        }
    }
    try {
        Logger::getInstance().log("Renderer application started", Logger::Level::INFO);
        TargetManager::init();
        RenderEngine engine;
        SceneLoader loader;
        Scene scene = loader.loadGltf(input);
        loader.addPlane(scene);
        std::shared_ptr<RenderTarget> egl = TargetManager::getInstance().createEGLTarget(width, height);
        engine.renderFrame(*egl, scene, samples);

        auto* eglTarget = dynamic_cast<EglTarget*>(egl.get());
        if (eglTarget) {
            RenderTarget::ContextGuard guard(*egl);
            Logger::getInstance().log("Writing into output.png", Logger::Level::DEBUG);
            utils::writeToPng(egl->getBufferData<uint8_t>(egl->getOutputTexture()), egl->getWidth(), egl->getHeight(),
                              4, std::format("{}.png", output));
            if (debugImages) {
                Logger::getInstance().log("Writing into output_raw.png", Logger::Level::DEBUG);
                utils::writeToPng(egl->getBufferData<float>(egl->getRawTexture()), egl->getWidth(), egl->getHeight(), 4,
                                  std::format("{}-raw.png", output));
                Logger::getInstance().log("Writing into output_albedo.png", Logger::Level::DEBUG);
                utils::writeToPng(egl->getBufferData<float>(egl->getAlbedoMap()), egl->getWidth(), egl->getHeight(), 4,
                                  std::format("{}-albedo.png", output));
                Logger::getInstance().log("Writing into output_normal.png", Logger::Level::DEBUG);
                utils::writeToPng(egl->getBufferData<float>(egl->getNormalMap()), egl->getWidth(), egl->getHeight(), 4,
                                  std::format("{}-normals.png", output));
            }
        }
        Logger::getInstance().log("Renderer application stopped successfully", Logger::Level::INFO);
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        Logger::getInstance().log("Application terminated due to error", Logger::Level::FATAL);
        return EXIT_FAILURE;
    }
}