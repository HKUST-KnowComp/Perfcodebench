#include "interface.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (dictionary.empty() || codes.empty() || iters <= 0) {
        return 0;
    }

    // Preprocess: assign unique integer ID to each distinct dictionary string, done once
    std::unordered_map<std::string, int> str_to_id;
    str_to_id.reserve(dictionary.size());
    std::vector<int> dict_index_to_id(dictionary.size());
    int next_id = 0;
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        auto it = str_to_id.find(s);
        if (it == str_to_id.end()) {
            int new_id = next_id++;
            str_to_id[s] = new_id;
            dict_index_to_id[i] = new_id;
        } else {
            dict_index_to_id[i] = it->second;
        }
    }

    // Generation counter trick: track last iteration each ID was seen, no reset needed between runs
    std::vector<int> last_seen(next_id, 0);
    uint64_t count = 0;

    for (int iter = 1; iter <= iters; ++iter) {
        uint64_t current_distinct = 0;
        for (uint16_t code : codes) {
            const int id = dict_index_to_id[static_cast<size_t>(code)];
            if (last_seen[id] != iter) {
                last_seen[id] = iter;
                current_distinct++;
            }
        }
        count = current_distinct;
    }

    return count;
}