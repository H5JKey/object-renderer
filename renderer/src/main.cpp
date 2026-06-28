#include <print>
#include "scene-loader.hpp"
#include <scene.hpp>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::println("Usage: {} <scene.json>", argv[0]);
        return EXIT_FAILURE;
    }
    std::string path = argv[1];
    try {
        JsonLoader loader;
        Scene scene = loader.load(path);
    } catch (const std::exception& e) {
        std::println(stderr, "{}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}