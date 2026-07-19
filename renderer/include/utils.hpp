#pragma once

#include <string>
#include <vector>

namespace utils {
std::string readFromFile(const std::string& path);
void rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb);
void rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba);
}  // namespace utils
