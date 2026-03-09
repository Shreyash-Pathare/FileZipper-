#include "BitReader.h"
#include <stdexcept>

BitReader::BitReader(std::ifstream& in)
    : inStream(in), buffer(0), bitCount(0), totalBits(0) {}

int BitReader::readBit() {
    if (bitCount == 0) {
        int byte = inStream.get();
        if (byte == EOF || inStream.eof()) return -1;
        buffer = (uint8_t)byte;
        bitCount = 8;
    }

    bitCount--;
    totalBits++;
    return (buffer >> bitCount) & 1;
}

int BitReader::readByte() {
    uint8_t result = 0;
    for (int i = 7; i >= 0; --i) {
        int bit = readBit();
        if (bit == -1) return -1;
        result |= (bit << i);
    }
    return result;
}

uint32_t BitReader::readUInt32() {
    uint32_t result = 0;
    for (int i = 3; i >= 0; --i) {
        int byte = readByte();
        if (byte == -1) throw std::runtime_error("Unexpected EOF reading uint32");
        result |= ((uint32_t)(uint8_t)byte << (i * 8));
    }
    return result;
}

uint64_t BitReader::readUInt64() {
    uint64_t result = 0;
    for (int i = 7; i >= 0; --i) {
        int byte = readByte();
        if (byte == -1) throw std::runtime_error("Unexpected EOF reading uint64");
        result |= ((uint64_t)(uint8_t)byte << (i * 8));
    }
    return result;
}

bool BitReader::isEOF() const {
    return inStream.eof();
}