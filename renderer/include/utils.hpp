#pragma once
#include <string>
#include <vector>

namespace utils {
    std::string readFromFile(const std::string& path);
    void rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb);
    void rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba);

    void rgbaToRgb(const std::vector<unsigned char>& rgba, std::vector<unsigned char>& rgb);
    void rgbToRgba(const std::vector<unsigned char>& rgb, std::vector<unsigned char>& rgba);

    void writeToPng(const std::vector<unsigned char>& pixels, int width, int height, int channels, const std::string& filename);
    void writeToPng(const std::vector<float>& pixels, int width, int height, int channels, const std::string& filename);
}