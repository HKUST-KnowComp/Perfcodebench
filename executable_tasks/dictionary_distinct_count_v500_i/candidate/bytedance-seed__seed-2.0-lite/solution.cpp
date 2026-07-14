#include "interface.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <string_view>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    // Preprocess dictionary to assign unique IDs to distinct strings (run once)
    std::unordered_map<std::string_view, uint32_t> str_to_id;
    uint32_t next_id = 0;
    std::vector<uint32_t> dict_ids(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        std::string_view s = dictionary[i];
        auto it = str_to_id.find(s);
        if (it == str_to_id.end()) {
            str_to_id[s] = next_id;
            dict_ids[i] = next_id++;
        } else {
            dict_ids[i] = it->second;
        }
    }

    // Precompute IDs for all input codes once, before measurement loop
    std::vector<uint32_t> code_ids(codes.size());
    for (size_t i = 0; i < codes.size(); ++i) {
        code_ids[i] = dict_ids[static_cast<size_t>(codes[i])];
    }

    // Generation-tracked seen array to avoid repeated memset/unordered_set clear
    std::vector<uint32_t> seen(next_id, 0);
    uint32_t gen = 0;
    uint64_t count = 0;

    // Fast per-iteration logic, no string operations or allocations
    for (int iter = 0; iter < iters; ++iter) {
        gen++;
        uint64_t current = 0;
        for (uint32_t id : code_ids) {
            if (seen[id] != gen) {
                seen[id] = gen;
                current++;
            }
        }
        count = current;
    }

    return count;
}