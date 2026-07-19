#include "denoiser.hpp"

#include <print>

Denoiser::Denoiser() {
    device = oidn::newDevice(oidn::DeviceType::CPU);
    device.commit();
}

void Denoiser::rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb) {
    size_t pixels = rgba.size() / 4;
    for (int i = 0; i < pixels; i++) {
        rgb[i * 3] = rgba[i * 4];
        rgb[i * 3 + 1] = rgba[i * 4 + 1];
        rgb[i * 3 + 2] = rgba[i * 4 + 2];
    }
}

void Denoiser::rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba) {
    size_t pixels = rgb.size() / 3;
    for (int i = 0; i < pixels; i++) {
        rgba[i * 4] = rgb[i * 3];
        rgba[i * 4 + 1] = rgb[i * 3 + 1];
        rgba[i * 4 + 2] = rgb[i * 3 + 2];
        rgba[i * 4 + 3] = 0;
    }
}

void Denoiser::denoise(RenderTarget& target) {
    int pixelCount = target.getWidth() * target.getHeight();
    size_t bufferSize = pixelCount * 3 * sizeof(float);

    std::vector<float> colorBufferData(pixelCount * 3, 0.0f);
    rgbaToRgb(target.getBufferData<float>(target.getRawTexture()), colorBufferData);
    oidn::BufferRef colorBuffer = device.newBuffer(colorBufferData.data(), bufferSize);

    std::vector<float> normalBufferData(pixelCount * 3, 0.0f);
    rgbaToRgb(target.getBufferData<float>(target.getNormalMap()), normalBufferData);
    oidn::BufferRef normalBuffer = device.newBuffer(normalBufferData.data(), bufferSize);

    std::vector<float> albedoBufferData(pixelCount * 3, 0.0f);
    rgbaToRgb(target.getBufferData<float>(target.getAlbedoMap()), albedoBufferData);
    oidn::BufferRef albedoBuffer = device.newBuffer(albedoBufferData.data(), bufferSize);

    std::vector<float> resultBufferData(pixelCount * 3, 0.0f);
    oidn::BufferRef outputBuffer = device.newBuffer(resultBufferData.data(), bufferSize);

    oidn::FilterRef filter = device.newFilter("RT");
    filter.setImage("color", colorBuffer, oidn::Format::Float3, target.getWidth(), target.getHeight());
    filter.setImage("normal", normalBuffer, oidn::Format::Float3, target.getWidth(), target.getHeight());
    filter.setImage("albedo", albedoBuffer, oidn::Format::Float3, target.getWidth(), target.getHeight());
    filter.setImage("output", outputBuffer, oidn::Format::Float3, target.getWidth(), target.getHeight());
    filter.set("hdr", true);
    filter.commit();
    filter.execute();

    const char* errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None) std::println("Denoising failed. Error: {}", errorMessage);

    std::vector<float> denoisedImageData(pixelCount * 4, 0);
    rgbToRgba(resultBufferData, denoisedImageData);
    target.setBufferData(target.getDenoisedTexture(), denoisedImageData);
}