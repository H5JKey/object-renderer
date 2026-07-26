#include <gtest/gtest.h>

#include <stdexcept>
#include <utils.hpp>

template <typename T>
class ImageReadWriteTest : public ::testing::Test {
   protected:
    int width, height, channels;
    int resultWidth, resultHeight, resultChannels;
    std::vector<uint8_t> result;
    std::vector<uint8_t> expected;
    std::vector<T> pixels;
    std::filesystem::path path;

    void TearDown() override { std::filesystem::remove(path); }
};

using MyTypes = ::testing::Types<uint8_t, float>;
TYPED_TEST_SUITE(ImageReadWriteTest, MyTypes);

TYPED_TEST(ImageReadWriteTest, roundTripWriteRead) {
    this->path = "test.png";
    this->width = 3;
    this->height = 1;
    this->channels = 4;
    if constexpr (std::is_same_v<TypeParam, float>) {
        this->pixels = {-0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.5f, 2.0f, 1.0f, 0.2f, 0.5f, 0.8f, 0.5f};
        this->expected = {0, 0, 127, 0, 255, 255, 255, 255, 51, 127, 204, 127};
    } else {
        this->pixels = {0, 0, 128, 0, 255, 255, 255, 255, 51, 128, 204, 128};
        this->expected = {0, 0, 128, 0, 255, 255, 255, 255, 51, 128, 204, 128};
    }
    /* Pixel data size mismatch */
    EXPECT_THROW(utils::writeToPng(this->pixels, this->width + 1, this->height, this->channels, this->path),
                 std::runtime_error);
    EXPECT_THROW(utils::writeToPng(this->pixels, this->width, this->height + 1, this->channels, this->path),
                 std::runtime_error);
    EXPECT_THROW(utils::writeToPng(this->pixels, this->width, this->height, (this->channels == 3) ? 4 : 3, this->path),
                 std::runtime_error);

    EXPECT_NO_THROW(utils::writeToPng(this->pixels, this->width, this->height, this->channels, this->path));
    EXPECT_NO_THROW(
        utils::readImage(this->path, this->resultWidth, this->resultHeight, this->resultChannels, this->result));

    EXPECT_EQ(this->result.size(), this->pixels.size());
    EXPECT_EQ(this->resultWidth, this->width);
    EXPECT_EQ(this->resultHeight, this->height);
    EXPECT_EQ(this->result.size(), this->expected.size());
    for (int i = 0; i < this->result.size(); i++) EXPECT_NEAR(this->result[i], this->expected[i], 1);
}

TYPED_TEST(ImageReadWriteTest, emptyImageWriteThrows) {
    this->path = "test.png";
    this->width = 0;
    this->height = 0;
    this->channels = 4;
    this->pixels = {};
    this->expected = {};

    /* Writing to empty file */
    EXPECT_THROW(utils::writeToPng(this->pixels, this->width, this->height, this->channels, this->path),
                 std::runtime_error);
}
