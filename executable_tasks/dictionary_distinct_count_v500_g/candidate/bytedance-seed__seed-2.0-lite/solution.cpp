#include "interface.h"

#include <unordered_map>
#include <string_view>
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (dictionary.empty() || codes.empty() || iters <= 0) {
        return 0;
    }

    // Preprocess unique IDs for all distinct dictionary strings
    std::unordered_map<std::string_view, uint32_t> str_to_id;
    str_to_id.reserve(dictionary.size());
    std::vector<uint32_t> dict_ids(dictionary.size());
    uint32_t next_id = 0;

    for (size_t i = 0; i < dictionary.size(); ++i) {
        std::string_view s(dictionary[i]);
        auto it = str_to_id.find(s);
        if (it == str_to_id.end()) {
            uint32_t new_id = next_id++;
            str_to_id[s] = new_id;
            dict_ids[i] = new_id;
        } else {
            dict_ids[i] = it->second;
        }
    }

    // Preprocess input codes into their unique IDs once
    std::vector<uint32_t> input_ids;
    input_ids.reserve(codes.size());
    for (uint16_t code : codes) {
        input_ids.push_back(dict_ids[static_cast<size_t>(code)]);
    }

    // Generation-tracked visited array to avoid costly reset operations
    std::vector<int> visited(next_id, 0);
    int current_gen = 1;
    uint64_t final_count = 0;

    // Execute all required iterations
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        for (uint32_t id : input_ids) {
            if (visited[id] != current_gen) {
                visited[id] = current_gen;
                count++;
            }
        }
        final_count = count;
        current_gen++;
    }

    return final_count;
}