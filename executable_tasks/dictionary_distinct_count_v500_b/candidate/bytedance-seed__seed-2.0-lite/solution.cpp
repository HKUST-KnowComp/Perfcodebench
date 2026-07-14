#include "interface.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    std::unordered_map<std::string, uint32_t> str_to_id;
    std::vector<uint32_t> code_to_id(dictionary.size());
    uint32_t next_id = 0;

    // Preprocess dictionary to map each code to a unique string ID (one-time cost)
    for (size_t c = 0; c < dictionary.size(); ++c) {
        const std::string& s = dictionary[c];
        auto it = str_to_id.find(s);
        if (it == str_to_id.end()) {
            str_to_id[s] = next_id;
            code_to_id[c] = next_id;
            next_id++;
        } else {
            code_to_id[c] = it->second;
        }
    }

    // Generation counter array to avoid clearing state every iteration
    std::vector<int> seen(next_id, 0);
    int current_generation = 1;
    uint64_t final_count = 0;

    // Run benchmarked iterations with minimal per-loop overhead
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        for (uint16_t code : codes) {
            uint32_t id = code_to_id[static_cast<size_t>(code)];
            if (seen[id] != current_generation) {
                count++;
                seen[id] = current_generation;
            }
        }
        current_generation++;
        final_count = count;
    }

    return final_count;
}