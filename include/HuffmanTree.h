#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <vector>
#include <memory>

// Huffman Tree Node
struct HuffmanNode {
    char ch;
    int freq;
    int id;   // insertion order — used for deterministic tie-breaking
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int f, int id_ = 0)
        : ch(c), freq(f), id(id_), left(nullptr), right(nullptr) {}

    HuffmanNode(int f, HuffmanNode* l, HuffmanNode* r, int id_ = 0)
        : ch('\0'), freq(f), id(id_), left(l), right(r) {}

    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

// Comparator for min-heap.
// FIX: tie-break by id (insertion order) so the heap is fully deterministic.
// Equal-freq nodes must always merge in the same order in both encoder and
// decoder — otherwise they produce different trees and decode produces garbage.
struct NodeCompare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        if (a->freq != b->freq) return a->freq > b->freq;
        return a->id > b->id;  // lower id = inserted earlier = higher priority
    }
};

class HuffmanTree {
public:
    HuffmanTree();
    ~HuffmanTree();

    // Build tree from frequency table
    void build(const std::unordered_map<char, int>& freqTable);

    // Generate code table (char -> binary string like "010110")
    std::unordered_map<char, std::string> generateCodes() const;

    // Getters
    HuffmanNode* getRoot() const { return root; }

    // Serialize tree to string (for storing in compressed file header)
    std::string serialize() const;

    // Deserialize tree from string
    void deserialize(const std::string& data);

    // Free memory
    void clear();

    // Print tree (debug)
    void printTree() const;

private:
    HuffmanNode* root;

    void generateCodesHelper(HuffmanNode* node, const std::string& code,
                              std::unordered_map<char, std::string>& codeTable) const;

    void serializeHelper(HuffmanNode* node, std::string& result) const;

    HuffmanNode* deserializeHelper(const std::string& data, int& index);

    void clearHelper(HuffmanNode* node);

    void printHelper(HuffmanNode* node, const std::string& prefix, bool isLeft) const;
};