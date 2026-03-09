#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

class FrequencyTable {
public:
    // Build frequency table from raw string data
    static std::unordered_map<char, int> build(const std::string& data);

    // Build frequency table from file path
    static std::unordered_map<char, int> buildFromFile(const std::string& filepath);

    // Print frequency table (for debugging/info)
    static void print(const std::unordered_map<char, int>& table);

    // Get sorted frequencies (ascending) for heap building
    static std::vector<std::pair<char, int>> getSorted(const std::unordered_map<char, int>& table);
};