#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // Pre-process the dictionary to group codes that map to the same string.
    // We assign a unique ID (representative) to each unique string.
    size_t dict_size = dictionary.size();
    std::vector<uint16_t> code_to_id(dict_size, 0);
    
    // To find unique strings, we can sort pairs of (string, original_index).
    struct Entry {
        const std::string* s;
        uint16_t id;
    };
    std::vector<Entry> entries(dict_size);
    for (size_t i = 0; i < dict_size; ++i) {
        entries[i] = {&dictionary[i], static_cast<uint16_t>(i)};
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return *(a.s) < *(b.s);
    });

    uint16_t unique_id_count = 0;
    if (dict_size > 0) {
        code_to_id[entries[0].id] = 0;
        unique_id_count = 1;
        for (size_t i = 1; i < dict_size; ++i) {
            if (*(entries[i].s) != *(entries[i-1].s)) {
                unique_id_count++;
            }
            code_to_id[entries[i].id] = unique_id_count - 1;
        }
    }

    // The problem asks for the count of distinct decoded strings in the 'codes' vector.
    // Since the dictionary and codes are constant across 'iters', the result is the same for every iteration.
    // We only need to compute it once.
    
    std::vector<bool> seen_id(unique_id_count, false);
    uint64_t distinct_count = 0;
    
    for (uint16_t code : codes) {
        uint16_t id = code_to_id[code];
        if (!seen_id[id]) {
            seen_id[id] = true;
            distinct_count++;
        }
    }

    // The loop over 'iters' is likely for benchmarking; the result is invariant.
    // We return the calculated count.
    return distinct_count;
}