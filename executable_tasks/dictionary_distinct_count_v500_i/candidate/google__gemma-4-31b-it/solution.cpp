#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // Pre-process the dictionary to map identical strings to the same ID.
    // This allows us to work with integers instead of strings in the loop.
    std::vector<uint16_t> canonical_map(dictionary.size());
    std::vector<uint16_t> unique_ids;
    
    // To efficiently find duplicates in the dictionary, we sort indices by string value.
    std::vector<uint32_t> sorted_indices(dictionary.size());
    std::iota(sorted_indices.begin(), sorted_indices.end(), 0);
    
    std::sort(sorted_indices.begin(), sorted_indices.end(), [&](uint32_t a, uint32_t b) {
        return dictionary[a] < dictionary[b];
    });

    if (!dictionary.empty()) {
        uint16_t current_id = 0;
        canonical_map[sorted_indices[0]] = current_id;
        for (size_t i = 1; i < sorted_indices.size(); ++i) {
            if (dictionary[sorted_indices[i]] != dictionary[sorted_indices[i - 1]]) {
                current_id++;
            }
            canonical_map[sorted_indices[i]] = current_id;
        }
        unique_ids.resize(current_id + 1);
    }

    uint64_t final_count = 0;
    // Use a boolean vector to track seen canonical IDs. 
    // Max size is dictionary.size(), which is at most 65536.
    std::vector<bool> seen(dictionary.size(), false);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(seen.begin(), seen.end(), false);
        uint64_t distinct_count = 0;
        for (uint16_t code : codes) {
            uint16_t id = canonical_map[code];
            if (!seen[id]) {
                seen[id] = true;
                distinct_count++;
            }
        }
        final_count = distinct_count;
    }

    return final_count;
}