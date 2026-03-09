#include "HuffmanEncoder.h"
#include "HuffmanDecoder.h"
#include "FrequencyTable.h"
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void printUsage(const std::string& progName) {
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║        FileZipper v1.0.0             ║\n";
    std::cout << "║   Huffman Encoding Compression Tool  ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << progName << " compress   <input>  <output.huff>\n";
    std::cout << "  " << progName << " decompress <input.huff> <output>\n";
    std::cout << "  " << progName << " info       <input.huff>\n";
    std::cout << "  " << progName << " tree       <input>\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << progName << " compress   document.txt  document.huff\n";
    std::cout << "  " << progName << " decompress document.huff restored.txt\n";
    std::cout << "  " << progName << " info       document.huff\n";
    std::cout << "  " << progName << " tree       document.txt\n\n";
}

void handleCompress(const std::string& input, const std::string& output) {
    if (!fs::exists(input)) {
        std::cerr << "Error: Input file does not exist: " << input << "\n";
        return;
    }

    std::cout << "Compressing: " << input << " → " << output << "\n";
    HuffmanEncoder encoder;
    if (encoder.compress(input, output)) {
        std::cout << "Compression successful!\n";
        encoder.printStats();
    } else {
        std::cerr << "Compression failed.\n";
    }
}

void handleDecompress(const std::string& input, const std::string& output) {
    if (!fs::exists(input)) {
        std::cerr << "Error: Compressed file does not exist: " << input << "\n";
        return;
    }

    std::cout << "Decompressing: " << input << " → " << output << "\n";
    HuffmanDecoder decoder;
    if (!decoder.decompress(input, output)) {
        std::cerr << "Decompression failed.\n";
    }
}

void handleInfo(const std::string& input) {
    if (!fs::exists(input)) {
        std::cerr << "Error: File does not exist: " << input << "\n";
        return;
    }

    auto size = fs::file_size(input);
    std::string ext = fs::path(input).extension().string();
    std::cout << "\n--- File Info: " << input << " ---\n";
    std::cout << "Size     : " << size << " bytes\n";
    std::cout << "Extension: " << (ext.empty() ? "(none)" : ext) << "\n";

    // FIX: don't try to show freq table for .huff binary files — shows garbage
    if (ext == ".huff" || ext == ".hz") {
        std::cout << "(Compressed file — use 'decompress' to restore)\n";
    } else {
        try {
            auto freqTable = FrequencyTable::buildFromFile(input);
            FrequencyTable::print(freqTable);
        } catch (...) {
            std::cout << "(Binary file — frequency analysis skipped)\n";
        }
    }
}

void handleTree(const std::string& input) {
    if (!fs::exists(input)) {
        std::cerr << "Error: File does not exist: " << input << "\n";
        return;
    }

    try {
        auto freqTable = FrequencyTable::buildFromFile(input);
        HuffmanTree tree;
        tree.build(freqTable);

        std::cout << "Huffman tree for: " << input << "\n";
        tree.printTree();

        auto codes = tree.generateCodes();
        std::cout << "--- Generated Codes ---\n";
        std::cout << "Char\tCode\t\tLength\n";
        std::cout << "--------------------------------\n";
        for (auto& [ch, code] : codes) {
            if (ch == '\n') std::cout << "\\n\t" << code << "\t\t" << code.size() << "\n";
            else if (ch == ' ') std::cout << "SPC\t" << code << "\t\t" << code.size() << "\n";
            else std::cout << ch << "\t" << code << "\t\t" << code.size() << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string command = argv[1];

    if (command == "compress" && argc == 4) {
        handleCompress(argv[2], argv[3]);
    } else if (command == "decompress" && argc == 4) {
        handleDecompress(argv[2], argv[3]);
    } else if (command == "info" && argc == 3) {
        handleInfo(argv[2]);
    } else if (command == "tree" && argc == 3) {
        handleTree(argv[2]);
    } else {
        std::cerr << "Invalid command or arguments.\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}