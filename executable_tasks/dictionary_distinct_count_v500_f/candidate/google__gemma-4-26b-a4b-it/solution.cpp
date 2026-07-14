#include "interface.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // Step 1: Pre-process dictionary to map each index to a unique ID representing its string content.
    // This handles cases where dictionary[i] == dictionary[j].
    std::vector<uint32_t> dict_to_canonical(dictionary.size());
    std::unordered_map<std::string, uint32_t> string_to_id;
    uint32_t next_id = 0;

    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        auto it = string_to_id.find(dictionary[i]);
        if (it == string_to_id.end()) {
            string_to_id[dictionary[i]] = next_id;
            dict_to_canonical[i] = next_id;
            next_id++;
        } else {
            dict_to_canonical[i] = it->second;
        }
    }

    uint64_t final_count = 0;
    // Pre-allocate set to avoid reallocations in the loop
    std::unordered_set<uint32_t> seen;
    seen.reserve(codes.size());

    for (int iter = 0; iter < iters; ++iter) {
        seen.clear();
        for (uint16_t code : codes) {
            // Map the code to its canonical string ID
            seen.insert(dict_to_canonical[static_cast<std::size_t>(code)]);
        }
        final_count = static_cast<uint64_t>(seen.size());
    }

    return final_count;
}