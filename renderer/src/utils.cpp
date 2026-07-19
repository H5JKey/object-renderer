#include "utils.hpp"

#include <format>
#include <fstream>
#include <sstream>

std::string utils::readFromFile(const std::string& path) {
    std::string source;
    std::fstream shaderFile(path);
    if (!shaderFile.is_open()) {
        throw std::runtime_error(std::format("failed to open file. Path: {}", path));
    }
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    source = buffer.str();
    return source;
}