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

    // Pre-process the dictionary to map each index to a unique ID based on string content.
    // This handles cases where the dictionary contains duplicate strings.
    std::vector<uint16_t> index_to_id(dictionary.size());
    std::unordered_map<std::string, uint16_t> string_to_id;
    uint16_t next_id = 0;

    for (size_t i = 0; i < dictionary.size(); ++i) {
        auto it = string_to_id.find(dictionary[i]);
        if (it == string_to_id.end()) {
            index_to_id[i] = next_id;
            string_to_id[dictionary[i]] = next_id++;
        } else {
            index_to_id[i] = it->second;
        }
    }

    uint64_t final_count = 0;
    // Use a boolean vector to track seen IDs. 
    // Since next_id is at most dictionary.size() (max 65535), this is efficient.
    std::vector<bool> seen(next_id, false);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(seen.begin(), seen.end(), false);
        uint64_t current_distinct = 0;
        
        for (uint16_t code : codes) {
            uint16_t id = index_to_id[code];
            if (!seen[id]) {
                seen[id] = true;
                current_distinct++;
            }
        }
        final_count = current_distinct;
    }

    return final_count;
}