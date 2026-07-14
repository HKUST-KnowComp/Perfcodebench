#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    uint64_t last_count = 0;
    size_t dict_size = dictionary.size();
    
    // Pre-allocate a presence vector to avoid reallocations.
    // Using a vector<bool> or a vector<char> for fast membership testing.
    std::vector<char> seen(dict_size, 0);
    std::vector<uint16_t> visited_indices;
    visited_indices.reserve(dict_size);

    for (int iter = 0; iter < iters; ++iter) {
        visited_indices.clear();
        uint64_t current_distinct = 0;

        for (uint16_t code : codes) {
            // The problem implies codes are valid indices into the dictionary.
            if (seen[code] == 0) {
                seen[code] = 1;
                visited_indices.push_back(code);
                current_distinct++;
            }
        }

        last_count = current_distinct;

        // Reset the seen vector efficiently by only touching indices we modified.
        for (uint16_t idx : visited_indices) {
            seen[idx] = 0;
        }
    }

    return last_count;
}