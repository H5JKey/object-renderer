#include "utils.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
    if (rgba.size() % 4 != 0) {
        throw std::runtime_error("RGBA buffer size must be multiple of 4");
    }
    if (rgb.size() % 3 != 0) {
        throw std::runtime_error("RGB buffer size must be multiple of 3");
    }
    if (rgba.size() / 4 != rgb.size() / 3) {
        throw std::runtime_error("Rgba and rgb buffers size mismatch");
    }
    size_t pixels = rgba.size() / 4;
    for (int i = 0; i < pixels; i++) {
        rgb[i * 3] = rgba[i * 4];
        rgb[i * 3 + 1] = rgba[i * 4 + 1];
        rgb[i * 3 + 2] = rgba[i * 4 + 2];
    }
}

void utils::rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba) {
    if (rgba.size() % 4 != 0) {
        throw std::runtime_error("RGBA buffer size must be multiple of 4");
    }
    if (rgb.size() % 3 != 0) {
        throw std::runtime_error("RGB buffer size must be multiple of 3");
    }
    if (rgba.size() / 4 != rgb.size() / 3) {
        throw std::runtime_error("Rgba and rgb buffers size mismatch");
    }
    size_t pixels = rgb.size() / 3;
    for (int i = 0; i < pixels; i++) {
        rgba[i * 4] = rgb[i * 3];
        rgba[i * 4 + 1] = rgb[i * 3 + 1];
        rgba[i * 4 + 2] = rgb[i * 3 + 2];
        rgba[i * 4 + 3] = 1;
    }
}

void utils::rgbaToRgb(const std::vector<uint8_t>& rgba, std::vector<uint8_t>& rgb) {
    if (rgba.size() % 4 != 0) {
        throw std::runtime_error("RGBA buffer size must be multiple of 4");
    }
    if (rgb.size() % 3 != 0) {
        throw std::runtime_error("RGB buffer size must be multiple of 3");
    }
    if (rgba.size() / 4 != rgb.size() / 3) {
        throw std::runtime_error("RGBA and RGB buffers size mismatch");
    }
    size_t pixels = rgba.size() / 4;
    for (int i = 0; i < pixels; i++) {
        rgb[i * 3] = rgba[i * 4];
        rgb[i * 3 + 1] = rgba[i * 4 + 1];
        rgb[i * 3 + 2] = rgba[i * 4 + 2];
    }
}

void utils::rgbToRgba(const std::vector<uint8_t>& rgb, std::vector<uint8_t>& rgba) {
    if (rgba.size() % 4 != 0) {
        throw std::runtime_error("RGBA buffer size must be multiple of 4");
    }
    if (rgb.size() % 3 != 0) {
        throw std::runtime_error("RGB buffer size must be multiple of 3");
    }
    if (rgba.size() / 4 != rgb.size() / 3) {
        throw std::runtime_error("RGBA and RGB buffers size mismatch");
    }
    size_t pixels = rgb.size() / 3;
    for (int i = 0; i < pixels; i++) {
        rgba[i * 4] = rgb[i * 3];
        rgba[i * 4 + 1] = rgb[i * 3 + 1];
        rgba[i * 4 + 2] = rgb[i * 3 + 2];
        rgba[i * 4 + 3] = 255;
    }
}

void utils::writeToPng(const std::vector<uint8_t>& pixels, int width, int height, int channels,
                       const std::string& filename) {
    if (pixels.size() != static_cast<size_t>(width * height * channels)) {
        throw std::runtime_error("Pixel data size mismatch");
    }
    std::clog << std::format("Writing into {}", filename) << std::endl;
    stbi_write_png(filename.c_str(), width, height, channels, pixels.data(), width * channels);
}

void utils::writeToPng(const std::vector<float>& pixels, int width, int height, int channels,
                       const std::string& filename) {
    if (pixels.size() != static_cast<size_t>(width * height * channels)) {
        throw std::runtime_error("Pixel data size mismatch");
    }
    std::clog << std::format("Writing into {}", filename) << std::endl;
    std::vector<unsigned char> normalizedPixels(width * height * channels);
    for (int i = 0; i < pixels.size(); i++)
        normalizedPixels[i] = static_cast<unsigned char>(std::min(std::max(pixels[i], 0.0f), 1.0f) * 255);
    stbi_write_png(filename.c_str(), width, height, channels, normalizedPixels.data(), width * channels);
}