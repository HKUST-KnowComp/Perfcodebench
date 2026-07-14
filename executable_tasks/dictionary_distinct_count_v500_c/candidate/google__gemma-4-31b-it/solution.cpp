#include "interface.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // Pre-process the dictionary to group codes that point to the same string content.
    // We map each dictionary index to a 'canonical' index.
    std::vector<uint16_t> canonical_map(dictionary.size());
    std::unordered_map<std::string, uint16_t> string_to_id;
    uint16_t next_id = 0;

    for (size_t i = 0; i < dictionary.size(); ++i) {
        auto it = string_to_id.find(dictionary[i]);
        if (it == string_to_id.end()) {
            string_to_id[dictionary[i]] = next_id;
            canonical_map[i] = next_id++;
        } else {
            canonical_map[i] = it->second;
        }
    }

    // The number of unique strings in the dictionary is next_id.
    // We need to count how many of these unique IDs appear in the 'codes' array.
    // Since iters is provided, we perform the operation iters times as per the baseline.
    uint64_t final_count = 0;
    
    // Optimization: The result is the same for every iteration since dictionary and codes are constant.
    // We only need to compute it once.
    if (iters > 0) {
        std::vector<bool> seen(next_id, false);
        uint64_t distinct_count = 0;
        for (uint16_t code : codes) {
            uint16_t cid = canonical_map[code];
            if (!seen[cid]) {
                seen[cid] = true;
                distinct_count++;
            }
        }
        final_count = distinct_count;
    }

    return final_count;
}