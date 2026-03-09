#include "HuffmanDecoder.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

HuffmanDecoder::HuffmanDecoder() {}

bool HuffmanDecoder::decompress(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Cannot open compressed file: " << inputFile << "\n";
        return false;
    }

    char magic[4];
    in.read(magic, 4);
    if (std::string(magic, 4) != std::string("HUF\x01", 4)) {
        std::cerr << "Invalid file format. Not a .huff file.\n";
        return false;
    }

    std::unordered_map<char, int> freqTable;
    size_t originalSize = 0;
    if (!readHeader(in, freqTable, originalSize)) {
        std::cerr << "Failed to read header.\n";
        return false;
    }

    tree.build(freqTable);

    // Read padding byte right after header, before any bit data
    int padding = (unsigned char)in.get();

    std::string decoded = decodeData(in, originalSize, padding);
    in.close();

    if (decoded.size() != originalSize) {
        std::cerr << "Warning: decoded " << decoded.size()
                  << " bytes, expected " << originalSize << "\n";
    }

    writeFile(outputFile, decoded);
    std::cout << "Decompressed successfully -> " << outputFile
              << " (" << decoded.size() << " bytes)\n";
    return true;
}

bool HuffmanDecoder::readHeader(std::ifstream& in,
                                 std::unordered_map<char, int>& freqTable,
                                 size_t& originalSize) {
    uint64_t origSize;
    in.read(reinterpret_cast<char*>(&origSize), sizeof(origSize));
    originalSize = (size_t)origSize;

    uint32_t uniqueCount;
    in.read(reinterpret_cast<char*>(&uniqueCount), sizeof(uniqueCount));

    for (uint32_t i = 0; i < uniqueCount; ++i) {
        char ch = (char)in.get();
        uint32_t freq;
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        freqTable[ch] = (int)freq;
    }

    return !freqTable.empty();
}

std::string HuffmanDecoder::decodeData(std::ifstream& in, size_t originalSize, int padding) {
    // Read all remaining bytes
    std::string bytes((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
    if (bytes.empty()) return "";

    // Encoder packs bits MSB-first: bit 0 of the bit-string goes to bit 7 of byte 0
    // Build bit string: for each byte read bits from MSB to LSB,
    // but strip 'padding' bits from the END of the last byte.
    std::string bitStream;
    bitStream.reserve(bytes.size() * 8);

    for (size_t i = 0; i < bytes.size(); i++) {
        unsigned char byte = (unsigned char)bytes[i];
        int validBits = (i == bytes.size() - 1) ? (8 - padding) : 8;
        for (int b = 7; b >= (8 - validBits); b--) {
            bitStream += ((byte >> b) & 1) ? '1' : '0';
        }
    }

    HuffmanNode* root = tree.getRoot();
    if (!root) return "";

    std::string result;
    result.reserve(originalSize);

    // Single unique character edge case
    if (root->isLeaf()) {
        while (result.size() < originalSize)
            result += root->ch;
        return result;
    }

    HuffmanNode* current = root;
    for (char bit : bitStream) {
        if (result.size() >= originalSize) break;
        current = (bit == '0') ? current->left : current->right;
        if (!current) {
            std::cerr << "Decode error: null node.\n";
            break;
        }
        if (current->isLeaf()) {
            result += current->ch;
            current = root;
        }
    }

    return result;
}

void HuffmanDecoder::writeFile(const std::string& filepath, const std::string& data) {
    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) throw std::runtime_error("Cannot write: " + filepath);
    out.write(data.c_str(), (std::streamsize)data.size());
}