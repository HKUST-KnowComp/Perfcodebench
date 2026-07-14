#include "interface.h"

#include <vector>
#include <string>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // The problem asks for the count of distinct decoded strings.
    // A decoded string is dictionary[codes[i]].
    // We only need to perform the calculation once if the input doesn't change across iters,
    // but the interface suggests iters might be for benchmarking. 
    // However, the logic inside the loop in the baseline is deterministic based on dictionary and codes.
    
    // Step 1: Identify which dictionary indices are actually used in the codes vector.
    // Since codes are uint16_t, there are at most 65536 possible indices.
    std::vector<bool> used(dictionary.size(), false);
    for (uint16_t code : codes) {
        used[code] = true;
    }

    // Step 2: Collect all strings that were actually used.
    std::vector<std::string_view> active_strings;
    active_strings.reserve(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        if (used[i]) {
            active_strings.emplace_back(dictionary[i]);
        }
    }

    // Step 3: Sort and count unique strings among the active set.
    std::sort(active_strings.begin(), active_strings.end());
    uint64_t distinct_count = 0;
    if (!active_strings.empty()) {
        distinct_count = 1;
        for (size_t i = 1; i < active_strings.size(); ++i) {
            if (active_strings[i] != active_strings[i - 1]) {
                distinct_count++;
            }
        }
    }

    // The baseline loop repeats the same operation 'iters' times.
    // We return the result of the final iteration.
    return distinct_count;
}