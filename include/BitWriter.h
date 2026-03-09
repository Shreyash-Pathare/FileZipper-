#pragma once
#include <fstream>
#include <cstdint>
#include <string>

// Writes individual bits to an output file stream
// Buffers bits into bytes before writing
class BitWriter {
public:
    explicit BitWriter(std::ofstream& out);
    ~BitWriter();

    // Write a single bit (true = 1, false = 0)
    void writeBit(bool bit);

    // Write a string of '0'/'1' characters as bits
    void writeBits(const std::string& bits);

    // Write a full byte
    void writeByte(uint8_t byte);

    // Write a 32-bit unsigned integer (for storing sizes)
    void writeUInt32(uint32_t value);

    // Write a 64-bit unsigned integer
    void writeUInt64(uint64_t value);

    // Flush remaining bits (pads with zeros to complete the last byte)
    // Returns the number of padding bits added
    int flush();

    // Returns total bits written (excluding padding)
    size_t getTotalBits() const { return totalBits; }

private:
    std::ofstream& outStream;
    uint8_t buffer;      // current byte being built
    int bitCount;        // how many bits are in buffer
    size_t totalBits;    // total bits written (before flush)
};