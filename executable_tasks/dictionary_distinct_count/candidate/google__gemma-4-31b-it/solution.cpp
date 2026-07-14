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
    if (dictionary.empty()) return 0;

    // Pre-process the dictionary to group identical strings.
    // We map each dictionary index to a canonical ID.
    size_t dict_size = dictionary.size();
    std::vector<uint16_t> canonical_ids(dict_size);
    
    // To find duplicates in the dictionary efficiently:
    // We store indices and sort them based on the string content.
    std::vector<size_t> indices(dict_size);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [&](size_t a, size_t b) {
        return dictionary[a] < dictionary[b];
    });

    uint16_t current_id = 0;
    canonical_ids[indices[0]] = current_id;
    for (size_t i = 1; i < dict_size; ++i) {
        if (dictionary[indices[i]] != dictionary[indices[i - 1]]) {
            current_id++;
        }
        canonical_ids[indices[i]] = current_id;
    }

    uint16_t num_unique_dict_entries = current_id + 1;
    std::vector<uint8_t> seen(num_unique_dict_entries);
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(seen.begin(), seen.end(), 0);
        uint64_t distinct_in_iter = 0;
        
        for (uint16_t code : codes) {
            uint16_t cid = canonical_ids[code];
            if (!seen[cid]) {
                seen[cid] = 1;
                distinct_in_iter++;
            }
        }
        count = distinct_in_iter;
    }

    return count;
}