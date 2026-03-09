#include "HuffmanEncoder.h"
#include "FrequencyTable.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

HuffmanEncoder::HuffmanEncoder() : stats{} {}

bool HuffmanEncoder::compress(const std::string& inputFile, const std::string& outputFile) {
    std::string data;
    try {
        data = readFile(inputFile);
    } catch (const std::exception& e) {
        std::cerr << "Error reading input: " << e.what() << "\n";
        return false;
    }

    if (data.empty()) {
        std::cerr << "Input file is empty.\n";
        return false;
    }

    stats.originalSize = data.size();

    auto freqTable = FrequencyTable::build(data);
    stats.uniqueCharacters = (int)freqTable.size();

    tree.build(freqTable);
    auto codeTable = tree.generateCodes();

    std::ofstream out(outputFile, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Cannot open output file: " << outputFile << "\n";
        return false;
    }

    writeHeader(out, freqTable, data.size());
    writeEncodedData(out, data, codeTable);
    out.close();

    // FIX: use filesystem::file_size instead of opening a second ifstream
    stats.compressedSize = (size_t)fs::file_size(outputFile);
    stats.compressionRatio = 100.0 * (1.0 - (double)stats.compressedSize / stats.originalSize);

    return true;
}

std::string HuffmanEncoder::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("Cannot open: " + filepath);
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

void HuffmanEncoder::writeHeader(std::ofstream& out,
                                  const std::unordered_map<char, int>& freqTable,
                                  size_t originalSize) {
    // Magic bytes
    out.write("HUF\x01", 4);

    // Original file size (8 bytes)
    uint64_t origSize = (uint64_t)originalSize;
    out.write(reinterpret_cast<const char*>(&origSize), sizeof(origSize));

    // Unique character count (4 bytes)
    uint32_t uniqueCount = (uint32_t)freqTable.size();
    out.write(reinterpret_cast<const char*>(&uniqueCount), sizeof(uniqueCount));

    // Frequency entries: char (1 byte) + freq (4 bytes)
    for (auto& [ch, freq] : freqTable) {
        out.put(ch);
        uint32_t f = (uint32_t)freq;
        out.write(reinterpret_cast<const char*>(&f), sizeof(f));
    }
}

void HuffmanEncoder::writeEncodedData(std::ofstream& out,
                                       const std::string& data,
                                       const std::unordered_map<char, std::string>& codeTable) {
    // FIX: build full bit string in memory first so we know the padding BEFORE
    // writing. Old code used seekp() placeholder which is unreliable on binary
    // streams across platforms and caused corrupt decompression.
    std::string bitStream;
    bitStream.reserve(data.size() * 4);
    for (unsigned char c : data) {
        bitStream += codeTable.at((char)c);
    }

    // Padding needed to fill the last byte
    int padding = (int)((8 - (bitStream.size() % 8)) % 8);

    // Write padding count first — decoder reads this byte before the bit data
    out.put((char)padding);

    // Pack bits into bytes and write
    for (size_t i = 0; i < bitStream.size(); i += 8) {
        uint8_t byte = 0;
        for (int b = 0; b < 8; b++) {
            byte <<= 1;
            if (i + b < bitStream.size() && bitStream[i + b] == '1')
                byte |= 1;
        }
        out.put((char)byte);
    }
}

void HuffmanEncoder::printStats() const {
    std::cout << "\n========= Compression Stats =========\n";
    std::cout << "Original size    : " << stats.originalSize << " bytes\n";
    std::cout << "Compressed size  : " << stats.compressedSize << " bytes\n";
    std::cout << "Compression ratio: " << std::fixed << std::setprecision(2)
              << stats.compressionRatio << "%\n";
    std::cout << "Unique characters: " << stats.uniqueCharacters << "\n";
    std::cout << "=====================================\n\n";
}