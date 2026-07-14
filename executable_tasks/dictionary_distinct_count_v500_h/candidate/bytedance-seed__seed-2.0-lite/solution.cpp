#include "interface.h"

#include <vector>
#include <unordered_map>
#include <string>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    // Preprocess dictionary to assign unique integer ID to each unique string (run once)
    std::unordered_map<std::string, int> str_to_id;
    std::vector<int> code_to_id(dictionary.size());
    int next_id = 0;
    for (size_t c = 0; c < dictionary.size(); ++c) {
        const std::string& s = dictionary[c];
        if (!str_to_id.contains(s)) {
            str_to_id[s] = next_id++;
        }
        code_to_id[c] = str_to_id[s];
    }

    // Precompute IDs for all input codes once, since input is fixed across all iterations
    std::vector<int> code_ids;
    code_ids.reserve(codes.size());
    for (uint16_t code : codes) {
        code_ids.push_back(code_to_id[static_cast<size_t>(code)]);
    }

    // Generation-counter seen array to avoid expensive full vector resets per iteration
    std::vector<int> seen(next_id, 0);
    int current_generation = 0;
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        current_generation++;
        int count = 0;
        for (int id : code_ids) {
            if (seen[id] != current_generation) {
                seen[id] = current_generation;
                count++;
            }
        }
        final_count = static_cast<uint64_t>(count);
    }

    return final_count;
}