#include "HuffmanTree.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>

HuffmanTree::HuffmanTree() : root(nullptr) {}
HuffmanTree::~HuffmanTree() { clear(); }

void HuffmanTree::build(const std::unordered_map<char, int>& freqTable) {
    clear();
    if (freqTable.empty())
        throw std::runtime_error("Frequency table is empty.");

    // FIX: sort entries by (freq, char) before inserting into the heap so that
    // leaf nodes get stable, deterministic ids. Without this, unordered_map
    // iteration produces a different insertion order each run, the heap
    // tie-breaks differently, and encoder/decoder build different trees ->
    // decompression outputs garbage.
    std::vector<std::pair<int,char>> sorted;
    sorted.reserve(freqTable.size());
    for (auto& [ch, freq] : freqTable) sorted.push_back({freq, ch});
    std::sort(sorted.begin(), sorted.end()); // asc by freq, then by char

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, NodeCompare> minHeap;
    int nextId = 0;
    for (auto& [freq, ch] : sorted)
        minHeap.push(new HuffmanNode(ch, freq, nextId++));

    // Single unique character: root is the leaf itself
    if (minHeap.size() == 1) {
        root = minHeap.top(); minHeap.pop();
        return;
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left  = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();
        minHeap.push(new HuffmanNode(left->freq + right->freq, left, right, nextId++));
    }
    root = minHeap.top();
}

std::unordered_map<char, std::string> HuffmanTree::generateCodes() const {
    std::unordered_map<char, std::string> codeTable;
    if (!root) return codeTable;
    if (root->isLeaf()) { codeTable[root->ch] = "0"; return codeTable; }
    generateCodesHelper(root, "", codeTable);
    return codeTable;
}

void HuffmanTree::generateCodesHelper(HuffmanNode* node, const std::string& code,
                                       std::unordered_map<char, std::string>& codeTable) const {
    if (!node) return;
    if (node->isLeaf()) { codeTable[node->ch] = code.empty() ? "0" : code; return; }
    generateCodesHelper(node->left,  code + "0", codeTable);
    generateCodesHelper(node->right, code + "1", codeTable);
}

std::string HuffmanTree::serialize() const {
    std::string result;
    serializeHelper(root, result);
    return result;
}

void HuffmanTree::serializeHelper(HuffmanNode* node, std::string& result) const {
    if (!node) return;
    if (node->isLeaf()) { result += '1'; result += node->ch; return; }
    result += '0';
    serializeHelper(node->left, result);
    serializeHelper(node->right, result);
}

void HuffmanTree::deserialize(const std::string& data) {
    clear(); int index = 0;
    root = deserializeHelper(data, index);
}

HuffmanNode* HuffmanTree::deserializeHelper(const std::string& data, int& index) {
    if (index >= (int)data.size()) return nullptr;
    if (data[index] == '1') {
        index++;
        char ch = data[index++];
        return new HuffmanNode(ch, 0);
    }
    index++;
    HuffmanNode* left  = deserializeHelper(data, index);
    HuffmanNode* right = deserializeHelper(data, index);
    return new HuffmanNode(0, left, right);
}

void HuffmanTree::clear() { clearHelper(root); root = nullptr; }

void HuffmanTree::clearHelper(HuffmanNode* node) {
    if (!node) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}

void HuffmanTree::printTree() const {
    std::cout << "\n--- Huffman Tree ---\n";
    printHelper(root, "", false);
    std::cout << "--------------------\n\n";
}

void HuffmanTree::printHelper(HuffmanNode* node, const std::string& prefix, bool isLeft) const {
    if (!node) return;
    std::cout << prefix << (isLeft ? "├── " : "└── ");
    if (node->isLeaf()) {
        if (node->ch == '\n') std::cout << "[\\n] (" << node->freq << ")\n";
        else if (node->ch == ' ') std::cout << "[SPACE] (" << node->freq << ")\n";
        else std::cout << "[" << node->ch << "] (" << node->freq << ")\n";
    } else {
        std::cout << "(internal: " << node->freq << ")\n";
    }
    printHelper(node->left,  prefix + (isLeft ? "│   " : "    "), true);
    printHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
}