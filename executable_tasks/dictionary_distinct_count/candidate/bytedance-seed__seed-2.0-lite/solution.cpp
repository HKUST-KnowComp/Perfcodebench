#include "interface.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    // Preprocess dictionary to assign unique IDs to each unique string
    std::unordered_map<std::string, int> str_to_id;
    int next_id = 0;
    std::vector<int> code_lookup(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        if (auto it = str_to_id.find(s); it == str_to_id.end()) {
            str_to_id[s] = next_id++;
        }
        code_lookup[i] = str_to_id[s];
    }

    // Precompute IDs for all input codes once, outside the hot iteration loop
    std::vector<int> code_ids(codes.size());
    for (size_t i = 0; i < codes.size(); ++i) {
        code_ids[i] = code_lookup[static_cast<size_t>(codes[i])];
    }

    // Generation counter to avoid resetting the entire seen array every iteration
    std::vector<uint32_t> last_seen(next_id, 0);
    uint32_t current_generation = 0;
    uint64_t final_count = 0;

    // Hot loop running all required iterations
    for (int iter = 0; iter < iters; ++iter) {
        current_generation++;
        int count = 0;
        for (int id : code_ids) {
            if (last_seen[id] != current_generation) {
                last_seen[id] = current_generation;
                count++;
            }
        }
        final_count = static_cast<uint64_t>(count);
    }

    return final_count;
}