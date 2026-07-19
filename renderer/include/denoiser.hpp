#pragma once
#include <OpenImageDenoise/oidn.hpp>

#include "render-target.hpp"

class Denoiser {
   public:
    Denoiser();
    void denoise(RenderTarget& target);

   private:
    void rgbaToRgb(const std::vector<float>& rgba, std::vector<float>& rgb);
    void rgbToRgba(const std::vector<float>& rgb, std::vector<float>& rgba);

    oidn::DeviceRef device;
};