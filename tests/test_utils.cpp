#include <gtest/gtest.h>
#include "utils.h"

TEST(UtilsTest, TrimRemovesWhitespace) {
    std::string s = "   hello world   ";
    trim(s);
    EXPECT_EQ(s, "hello world");
}

TEST(UtilsTest, TrimHandlesEmptyString) {
    std::string s = "";
    trim(s);
    EXPECT_TRUE(s.empty());
}

TEST(UtilsTest, ToLowerConvertsCorrectly) {
    EXPECT_EQ(toLower("HeLLo"), "hello");
    EXPECT_EQ(toLower("PPSA02785"), "ppsa02785");
}

TEST(UtilsTest, ToLowerHandlesEmpty) {
    EXPECT_EQ(toLower(""), "");
}
