#include "BitWriter.h"
#include <stdexcept>

BitWriter::BitWriter(std::ofstream& out)
    : outStream(out), buffer(0), bitCount(0), totalBits(0) {}

BitWriter::~BitWriter() {
    // Don't auto-flush here — caller must call flush() explicitly
}

void BitWriter::writeBit(bool bit) {
    buffer = (buffer << 1) | (bit ? 1 : 0);
    bitCount++;
    totalBits++;

    if (bitCount == 8) {
        outStream.put((char)buffer);
        buffer = 0;
        bitCount = 0;
    }
}

void BitWriter::writeBits(const std::string& bits) {
    for (char c : bits) {
        if (c == '1') writeBit(true);
        else if (c == '0') writeBit(false);
        else throw std::invalid_argument("writeBits: invalid character in bit string");
    }
}

void BitWriter::writeByte(uint8_t byte) {
    for (int i = 7; i >= 0; --i) {
        writeBit((byte >> i) & 1);
    }
}

void BitWriter::writeUInt32(uint32_t value) {
    for (int i = 3; i >= 0; --i) {
        writeByte((value >> (i * 8)) & 0xFF);
    }
}

void BitWriter::writeUInt64(uint64_t value) {
    for (int i = 7; i >= 0; --i) {
        writeByte((value >> (i * 8)) & 0xFF);
    }
}

int BitWriter::flush() {
    if (bitCount == 0) return 0;

    int padding = 8 - bitCount;
    buffer <<= padding; // pad remaining bits with zeros
    outStream.put((char)buffer);
    buffer = 0;
    bitCount = 0;
    return padding;
}