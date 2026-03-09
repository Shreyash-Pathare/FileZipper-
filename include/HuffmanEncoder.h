#pragma once
#include "HuffmanTree.h"
#include <string>
#include <unordered_map>

struct CompressionStats {
    size_t originalSize;
    size_t compressedSize;
    double compressionRatio;
    int uniqueCharacters;
};

class HuffmanEncoder {
public:
    HuffmanEncoder();

    // Main compress function: reads inputFile, writes to outputFile
    bool compress(const std::string& inputFile, const std::string& outputFile);

    // Get stats from last compression
    CompressionStats getStats() const { return stats; }

    // Print compression stats
    void printStats() const;

private:
    HuffmanTree tree;
    CompressionStats stats;

    std::string readFile(const std::string& filepath);

    void writeHeader(std::ofstream& out, const std::unordered_map<char, int>& freqTable,
                     size_t originalSize);

    void writeEncodedData(std::ofstream& out,
                          const std::string& data,
                          const std::unordered_map<char, std::string>& codeTable);
};