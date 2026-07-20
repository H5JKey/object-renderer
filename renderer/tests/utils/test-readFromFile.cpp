#include <gtest/gtest.h>

#include <fstream>
#include <stdexcept>

#include "utils.hpp"

TEST(Utils, ReadFromFile) {
    /* Failed to open file. Path: non/existent/file.txt*/
    EXPECT_THROW(utils::readFromFile("non/existent/file.txt"), std::runtime_error);

    std::string text = R"(
    #include <iostream>

    int main() {
        std::cout << "Hello, world!" << std::endl;
        return 0;
    }
    )";

    std::ofstream file("test.txt");
    ASSERT_TRUE(file.is_open());
    file << text;
    file.close();

    std::string result;
    EXPECT_NO_THROW(result = utils::readFromFile("test.txt"));
    EXPECT_EQ(result, text);

    std::remove("test.txt");
}
