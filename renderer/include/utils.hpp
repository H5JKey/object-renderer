#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace utils {
std::string readFromFile(const std::filesystem::path& path);
void rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb);
void rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba);

void rgbaToRgb(const std::vector<uint8_t>& rgba, std::vector<uint8_t>& rgb);
void rgbToRgba(const std::vector<uint8_t>& rgb, std::vector<uint8_t>& rgba);

void writeToPng(const std::vector<uint8_t>& pixels, int width, int height, int channels,
                const std::filesystem::path& path);
void writeToPng(const std::vector<float>& pixels, int width, int height, int channels,
                const std::filesystem::path& path);

void readPng(const std::filesystem::path& filename, int& width, int& height, int& channels,
             std::vector<uint8_t>& result);
}  // namespace utils