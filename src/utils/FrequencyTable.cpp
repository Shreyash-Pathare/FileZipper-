#include "FrequencyTable.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

std::unordered_map<char, int> FrequencyTable::build(const std::string& data) {
    std::unordered_map<char, int> table;
    for (unsigned char c : data) {
        table[c]++;
    }
    return table;
}

std::unordered_map<char, int> FrequencyTable::buildFromFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

    return build(content);
}

void FrequencyTable::print(const std::unordered_map<char, int>& table) {
    std::vector<std::pair<char, int>> sorted = getSorted(table);

    std::cout << "\n--- Frequency Table ---\n";
    std::cout << "Char\tASCII\tFrequency\n";
    std::cout << "-------------------------------\n";
    for (auto& [ch, freq] : sorted) {
        if (ch == '\n') std::cout << "\\n\t" << (int)ch << "\t" << freq << "\n";
        else if (ch == '\t') std::cout << "\\t\t" << (int)ch << "\t" << freq << "\n";
        else if (ch == ' ')  std::cout << "SPACE\t" << (int)ch << "\t" << freq << "\n";
        else std::cout << ch << "\t" << (int)ch << "\t" << freq << "\n";
    }
    std::cout << "-------------------------------\n";
    std::cout << "Unique chars: " << table.size() << "\n\n";
}

std::vector<std::pair<char, int>> FrequencyTable::getSorted(const std::unordered_map<char, int>& table) {
    std::vector<std::pair<char, int>> sorted(table.begin(), table.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return a.second < b.second; // ascending by frequency
    });
    return sorted;
}