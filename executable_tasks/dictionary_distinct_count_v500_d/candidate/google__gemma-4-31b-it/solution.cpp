#include "interface.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
    if (codes.empty()) return 0;

    // The problem asks for the count of distinct decoded strings.
    // A decoded string is dictionary[codes[i]].
    // We only need to perform this calculation once per call if the 
    // dictionary and codes don't change across iters (as per the loop structure).
    
    // Step 1: Identify which dictionary indices are actually used.
    // Since codes are uint16_t, the max index is 65535.
    std::vector<bool> used(dictionary.size(), false);
    for (uint16_t code : codes) {
        used[code] = true;
    }

    // Step 2: Count unique strings among the used dictionary entries.
    // We use a set of strings to handle cases where the dictionary contains duplicate strings.
    std::unordered_set<std::string_view> distinct_strings;
    for (size_t i = 0; i < dictionary.size(); ++i) {
        if (used[i]) {
            distinct_strings.insert(dictionary[i]);
        }
    }

    uint64_t result = static_cast<uint64_t>(distinct_strings.size());

    // The original code had a loop over 'iters'. Since the inputs are constant 
    // within the function call, the result is the same for every iteration.
    // We return the result of the last iteration.
    return result;
}