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

    // Pre-process the dictionary to map identical strings to the same ID.
    // This ensures that if dictionary[i] == dictionary[j], they are treated as the same entity.
    std::vector<uint16_t> canonical_ids(dictionary.size());
    std::unordered_map<std::string, uint16_t> string_to_id;
    uint16_t next_id = 0;
    for (size_t i = 0; i < dictionary.size(); ++i) {
        auto it = string_to_id.find(dictionary[i]);
        if (it == string_to_id.end()) {
            canonical_ids[i] = next_id;
            string_to_id[dictionary[i]] = next_id++;
        } else {
            canonical_ids[i] = it->second;
        }
    }

    // We only need to perform the distinct count once if the input doesn't change across iters.
    // The problem statement implies 'iters' is for benchmarking, but the logic is the same.
    // We use a boolean vector to mark which canonical IDs are present in the codes array.
    std::vector<bool> present(next_id, false);
    uint64_t distinct_count = 0;

    // The loop is kept to satisfy the interface, though the result is constant across iters.
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(present.begin(), present.end(), false);
        distinct_count = 0;
        for (uint16_t code : codes) {
            uint16_t cid = canonical_ids[code];
            if (!present[cid]) {
                present[cid] = true;
                distinct_count++;
            }
        }
    }

    return distinct_count;
}