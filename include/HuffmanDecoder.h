#pragma once
#include "HuffmanTree.h"
#include <string>
#include <unordered_map>

class HuffmanDecoder {
public:
    HuffmanDecoder();

    // Main decompress: reads .huff file, writes restored output
    bool decompress(const std::string& inputFile, const std::string& outputFile);

private:
    HuffmanTree tree;

    // Read header: rebuild freq table + original size
    bool readHeader(std::ifstream& in,
                    std::unordered_map<char, int>& freqTable,
                    size_t& originalSize);

    // FIX: padding param added — must be known before decoding begins
    std::string decodeData(std::ifstream& in, size_t originalSize, int padding);

    void writeFile(const std::string& filepath, const std::string& data);
};