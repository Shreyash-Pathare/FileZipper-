#include <gtest/gtest.h>
#include "HuffmanTree.h"
#include "FrequencyTable.h"
#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// ─── FrequencyTable Tests ───────────────────────────────────────────────────

TEST(FrequencyTableTest, BasicBuild) {
    std::string data = "aabbcc";
    auto table = FrequencyTable::build(data);
    EXPECT_EQ(table['a'], 2);
    EXPECT_EQ(table['b'], 2);
    EXPECT_EQ(table['c'], 2);
    EXPECT_EQ(table.size(), 3u);
}

TEST(FrequencyTableTest, SingleChar) {
    auto table = FrequencyTable::build("aaaa");
    EXPECT_EQ(table['a'], 4);
    EXPECT_EQ(table.size(), 1u);
}

TEST(FrequencyTableTest, EmptyString) {
    auto table = FrequencyTable::build("");
    EXPECT_TRUE(table.empty());
}

TEST(FrequencyTableTest, AllUniqueChars) {
    auto table = FrequencyTable::build("abcd");
    EXPECT_EQ(table.size(), 4u);
    for (char c : std::string("abcd")) {
        EXPECT_EQ(table[c], 1);
    }
}

// ─── HuffmanTree Tests ──────────────────────────────────────────────────────

TEST(HuffmanTreeTest, BuildAndGenerateCodes) {
    std::unordered_map<char, int> freq = {{'a', 5}, {'b', 3}, {'c', 1}};
    HuffmanTree tree;
    tree.build(freq);
    auto codes = tree.generateCodes();

    // All characters must have a code
    EXPECT_TRUE(codes.count('a'));
    EXPECT_TRUE(codes.count('b'));
    EXPECT_TRUE(codes.count('c'));

    // Higher frequency → shorter or equal code
    EXPECT_LE(codes['a'].size(), codes['c'].size());
}

TEST(HuffmanTreeTest, SingleUniqueChar) {
    std::unordered_map<char, int> freq = {{'x', 10}};
    HuffmanTree tree;
    tree.build(freq);
    auto codes = tree.generateCodes();
    EXPECT_TRUE(codes.count('x'));
    EXPECT_FALSE(codes['x'].empty());
}

TEST(HuffmanTreeTest, SerializeDeserialize) {
    std::unordered_map<char, int> freq = {{'a', 5}, {'b', 3}, {'c', 1}};
    HuffmanTree tree;
    tree.build(freq);

    std::string serialized = tree.serialize();
    EXPECT_FALSE(serialized.empty());

    HuffmanTree tree2;
    tree2.deserialize(serialized);

    // Both trees should produce identical code tables
    auto codes1 = tree.generateCodes();
    auto codes2 = tree2.generateCodes();
    EXPECT_EQ(codes1, codes2);
}

TEST(HuffmanTreeTest, CodesAreUnique) {
    std::unordered_map<char, int> freq = {
        {'a', 10}, {'b', 5}, {'c', 3}, {'d', 1}
    };
    HuffmanTree tree;
    tree.build(freq);
    auto codes = tree.generateCodes();

    // No two characters should share the same code
    std::unordered_map<std::string, char> reverse;
    for (auto& [ch, code] : codes) {
        EXPECT_FALSE(reverse.count(code)) << "Duplicate code: " << code;
        reverse[code] = ch;
    }
}

// ─── Encode / Decode Round-trip Tests ───────────────────────────────────────

class RoundTripTest : public ::testing::Test {
protected:
    std::string inputFile  = "/tmp/test_input.txt";
    std::string compFile   = "/tmp/test_output.huff";
    std::string decompFile = "/tmp/test_restored.txt";

    void TearDown() override {
        fs::remove(inputFile);
        fs::remove(compFile);
        fs::remove(decompFile);
    }

    void writeInput(const std::string& content) {
        std::ofstream f(inputFile, std::ios::binary);
        f << content;
    }

    std::string readOutput() {
        std::ifstream f(decompFile, std::ios::binary);
        std::ostringstream oss;
        oss << f.rdbuf();
        return oss.str();
    }
};

TEST_F(RoundTripTest, SimpleText) {
    std::string original = "hello world";
    writeInput(original);

    HuffmanEncoder enc;
    ASSERT_TRUE(enc.compress(inputFile, compFile));

    HuffmanDecoder dec;
    ASSERT_TRUE(dec.decompress(compFile, decompFile));

    EXPECT_EQ(readOutput(), original);
}

TEST_F(RoundTripTest, RepeatedChars) {
    std::string original(500, 'a');
    writeInput(original);

    HuffmanEncoder enc;
    ASSERT_TRUE(enc.compress(inputFile, compFile));

    HuffmanDecoder dec;
    ASSERT_TRUE(dec.decompress(compFile, decompFile));

    EXPECT_EQ(readOutput(), original);
}

TEST_F(RoundTripTest, AllASCIIChars) {
    std::string original;
    for (int i = 32; i < 127; ++i) original += (char)i;
    writeInput(original);

    HuffmanEncoder enc;
    ASSERT_TRUE(enc.compress(inputFile, compFile));

    HuffmanDecoder dec;
    ASSERT_TRUE(dec.decompress(compFile, decompFile));

    EXPECT_EQ(readOutput(), original);
}

TEST_F(RoundTripTest, LargeText) {
    std::string original;
    for (int i = 0; i < 10000; ++i) {
        original += "The quick brown fox jumps over the lazy dog. ";
    }
    writeInput(original);

    HuffmanEncoder enc;
    ASSERT_TRUE(enc.compress(inputFile, compFile));

    HuffmanDecoder dec;
    ASSERT_TRUE(dec.decompress(compFile, decompFile));

    EXPECT_EQ(readOutput(), original);
}

TEST_F(RoundTripTest, CompressionActuallyReducesSize) {
    std::string original(10000, 'a'); // highly compressible
    writeInput(original);

    HuffmanEncoder enc;
    ASSERT_TRUE(enc.compress(inputFile, compFile));
    auto stats = enc.getStats();

    EXPECT_LT(stats.compressedSize, stats.originalSize);
    EXPECT_GT(stats.compressionRatio, 0.0);
}

// ─── Main ───────────────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}