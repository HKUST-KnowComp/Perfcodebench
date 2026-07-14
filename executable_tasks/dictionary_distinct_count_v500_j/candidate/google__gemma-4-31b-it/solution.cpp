#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // Pre-process the dictionary to map each string to a unique ID.
    // This avoids repeated string comparisons and hashing.
    size_t dict_size = dictionary.size();
    std::vector<uint16_t> string_to_id(dict_size, 0);
    
    // We use a simple approach to find unique strings in the dictionary.
    // Since dict_size is likely small (uint16_t codes), we can afford this.
    std::vector<size_t> sorted_indices(dict_size);
    for (size_t i = 0; i < dict_size; ++i) sorted_indices[i] = i;

    std::sort(sorted_indices.begin(), sorted_indices.end(), [&](size_t a, size_t b) {
        return dictionary[a] < dictionary[b];
    });

    uint16_t unique_id = 0;
    if (dict_size > 0) {
        string_to_id[sorted_indices[0]] = 0;
        for (size_t i = 1; i < dict_size; ++i) {
            if (dictionary[sorted_indices[i]] != dictionary[sorted_indices[i - 1]]) {
                unique_id++;
            }
            string_to_id[sorted_indices[i]] = unique_id;
        }
    }

    uint64_t final_count = 0;
    // The problem asks for the count over 'iters'. 
    // Based on the baseline, it seems the result of the last iteration is returned.
    // The logic inside the loop is independent of the iteration index.
    
    // Use a boolean vector to mark which unique IDs are present in the codes.
    std::vector<bool> present(unique_id + 1, false);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(present.begin(), present.end(), false);
        uint64_t current_distinct = 0;
        for (uint16_t code : codes) {
            uint16_t id = string_to_id[code];
            if (!present[id]) {
                present[id] = true;
                current_distinct++;
            }
        }
        final_count = current_distinct;
    }

    return final_count;
}