#pragma once
#include <fstream>
#include <cstdint>

// Reads individual bits from an input file stream
class BitReader {
public:
    explicit BitReader(std::ifstream& in);

    // Read a single bit. Returns -1 on EOF.
    int readBit();

    // Read a full byte. Returns -1 on EOF.
    int readByte();

    // Read a 32-bit unsigned integer
    uint32_t readUInt32();

    // Read a 64-bit unsigned integer
    uint64_t readUInt64();

    // Check if end of stream
    bool isEOF() const;

    // Total bits read so far
    size_t getTotalBits() const { return totalBits; }

private:
    std::ifstream& inStream;
    uint8_t buffer;     // current byte being read
    int bitCount;       // how many bits remain in buffer
    size_t totalBits;   // total bits read
};