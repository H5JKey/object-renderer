#include <gtest/gtest.h>
#include <stb_image.h>

#include <stdexcept>
#include <utils.hpp>

void readPng(const std::string& filename, int& width, int& height, int& channels, std::vector<uint8_t>& result) {
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data) throw std::runtime_error("Failed to load");
    result.assign(data, data + width * height * channels);
    stbi_image_free(data);
};

template <typename T>
void testWriteToPng() {
    std::vector<T> pixels;
    std::string filename = "test.png";
    std::vector<uint8_t> expected;
    size_t size;

    if constexpr (std::is_same_v<T, float>) {
        pixels = {-0.5f, 0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 0.2f, 0.5f, 0.8f};
    } else {
        pixels = {0, 0, 128, 255, 255, 255, 51, 128, 204};
    }
    /* Pixel data size mismatch */
    EXPECT_THROW(utils::writeToPng(pixels, 3, 3, 3, filename), std::runtime_error);
    EXPECT_THROW(utils::writeToPng(pixels, 3, 1, 4, filename), std::runtime_error);
    EXPECT_THROW(utils::writeToPng(pixels, 1, 1, 3, filename), std::runtime_error);

    if constexpr (std::is_same_v<T, float>) {
        pixels = {-0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.5f, 2.0f, 1.0f, 0.2f, 0.5f, 0.8f, 0.5f};
    } else {
        pixels = {0, 0, 128, 0, 255, 255, 255, 255, 51, 128, 204, 128};
    }
    expected = {0, 0, 128, 0, 255, 255, 255, 255, 51, 128, 204, 128};

    EXPECT_NO_THROW(utils::writeToPng(pixels, 3, 1, 4, filename));

    int width, height, channels;
    std::vector<uint8_t> result;
    EXPECT_NO_THROW(readPng("test.png", width, height, channels, result));
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(width, 3);
    EXPECT_EQ(height, 1);
    EXPECT_EQ(channels, 4);
    for (int i = 0; i < result.size(); i++) EXPECT_NEAR(result[i], expected[i], 1);

    std::remove("test.png");
}

TEST(WriteToPng, uint8_t) { testWriteToPng<uint8_t>(); }
TEST(WriteToPng, float) { testWriteToPng<float>(); }
