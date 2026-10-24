#include <gtest/gtest.h>
#include "SynonymIndex.h"
#include <fstream>
#include <cstdio>

class SynonymIndexTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream f("test_synonyms.json");
        f << R"({
            "PPSA02387": ["a plague tale: innocence", "чумная история"],
            "PPSA07642": ["the last of us", "одни из нас"]
        })";
        f.close();
        ASSERT_TRUE(index.loadFromFile("test_synonyms.json"));
    }
    void TearDown() override {
        std::remove("test_synonyms.json");
    }
    SynonymIndex index;
};

TEST_F(SynonymIndexTest, LoadsSynonyms) {
    EXPECT_EQ(index.findCodeBySynonym("a plague tale: innocence"), "PPSA02387");
    EXPECT_EQ(index.findCodeBySynonym("чумная история"), "PPSA02387");
    EXPECT_EQ(index.findCodeBySynonym("одни из нас"), "PPSA07642");
}

TEST_F(SynonymIndexTest, CaseInsensitiveSearch) {
    EXPECT_EQ(index.findCodeBySynonym("A PLAGUE TALE: INNOCENCE"), "PPSA02387");
}

TEST_F(SynonymIndexTest, ReturnsEmptyForUnknown) {
    EXPECT_TRUE(index.findCodeBySynonym("unknown game").empty());
}

TEST_F(SynonymIndexTest, AddsNewSynonym) {
    index.addSynonyms("PPSA02387", {"новый синоним"});
    EXPECT_EQ(index.findCodeBySynonym("новый синоним"), "PPSA02387");
}

TEST_F(SynonymIndexTest, GetAllSynonymsNotEmpty) {
    auto all = index.getAllSynonyms();
    EXPECT_FALSE(all.empty());
}
