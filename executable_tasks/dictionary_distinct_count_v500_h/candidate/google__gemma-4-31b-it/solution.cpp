#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // The problem asks for the count of distinct decoded strings.
    // A decoded string is dictionary[codes[i]].
    // Two indices i and j result in the same decoded string if dictionary[codes[i]] == dictionary[codes[j]].
    
    // We only need to perform the calculation once if the input doesn't change across iters,
    // but the interface suggests iters might be for benchmarking. 
    // The logic inside the loop is the same every time.

    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // 1. Find all unique codes present in the input
        // Since codes are uint16_t, there are at most 65536 possible values.
        static bool present[65536];
        std::fill(present, present + 65536, false);
        
        for (uint16_t code : codes) {
            present[code] = true;
        }

        // 2. Collect the actual strings corresponding to the present codes
        // We use a set of strings to count distinct values among the used dictionary entries.
        std::unordered_set<std::string_view> distinct_strings;
        for (uint16_t i = 0; i < 65536; ++i) {
            if (present[i] && i < dictionary.size()) {
                distinct_strings.insert(dictionary[i]);
            }
        }
        final_count = distinct_strings.size();
    }

    return final_count;
}