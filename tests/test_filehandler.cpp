#include <gtest/gtest.h>
#include "FileHandler.h"
#include <fstream>
#include <cstdio>

class FileHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream f("test_games.json");
        f << R"([
            {
                "title": "Test Game",
                "platform": "PS5",
                "regionCodes": {
                    "Россия": "О",
                    "Польша": "PPSA12345"
                }
            },
            {
                "title": "Another Game",
                "platform": "PS5",
                "regionCodes": {
                    "Россия": "PPSA99999",
                    "Польша": "PPSA99999"
                },
                "regionLocalization": { "Россия": "subtitles" }
            }
        ])";
        f.close();
    }
    void TearDown() override {
        std::remove("test_games.json");
    }
};

TEST_F(FileHandlerTest, LoadsGamesCount) {
    auto games = loadGamesFromFile("test_games.json");
    ASSERT_EQ(games.size(), 2);
}

TEST_F(FileHandlerTest, ParsesTitleAndPlatform) {
    auto games = loadGamesFromFile("test_games.json");
    EXPECT_EQ(games[0].title, "Test Game");
    EXPECT_EQ(games[0].platform, "PS5");
}

TEST_F(FileHandlerTest, HandlesLocalizationMarkerAndFallback) {
    auto games = loadGamesFromFile("test_games.json");
    EXPECT_EQ(games[0].regionCodes.at("Россия"), "PPSA12345");
    EXPECT_EQ(games[0].regionLocalization.at("Россия"), "full");
}

TEST_F(FileHandlerTest, ParsesRegionLocalization) {
    auto games = loadGamesFromFile("test_games.json");
    EXPECT_EQ(games[1].regionLocalization.at("Россия"), "subtitles");
}

TEST_F(FileHandlerTest, ReturnsEmptyForInvalidFile) {
    auto games = loadGamesFromFile("nonexistent.json");
    EXPECT_TRUE(games.empty());
}
