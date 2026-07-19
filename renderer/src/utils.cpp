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

void utils::rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb) {
  size_t pixels = rgba.size() / 4;
  for (int i = 0; i < pixels; i++) {
    rgb[i * 3] = rgba[i * 4];
    rgb[i * 3 + 1] = rgba[i * 4 + 1];
    rgb[i * 3 + 2] = rgba[i * 4 + 2];
  }
}

void utils::rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba) {
  size_t pixels = rgb.size() / 3;
  for (int i = 0; i < pixels; i++) {
    rgba[i * 4] = rgb[i * 3];
    rgba[i * 4 + 1] = rgb[i * 3 + 1];
    rgba[i * 4 + 2] = rgb[i * 3 + 2];
    rgba[i * 4 + 3] = 0;
  }
}