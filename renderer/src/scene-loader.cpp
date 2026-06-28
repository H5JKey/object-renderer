#include "scene-loader.hpp"
#include <fstream>
#include <format>

using json = nlohmann::json;

Scene JsonLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("Cannot open file: {}",std::string(path)));
    }
    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::format("JSON parse error in {}: {}",std::string(path), e.what()));
    }
    return Scene();
}