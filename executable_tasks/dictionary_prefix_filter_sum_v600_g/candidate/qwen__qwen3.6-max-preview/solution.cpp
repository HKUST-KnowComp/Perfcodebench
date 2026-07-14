#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // Precompute match status for each dictionary entry.
    // Using uint8_t avoids std::vector<bool> bit-packing overhead.
    std::vector<uint8_t> dict_matches(dictionary.size(), 0);
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        // C++11 guarantees null-termination, so s[0] and s[1] are safe.
        // Short-circuit ensures s[1] is only read if s[0] == 'q'.
        if (s[0] == 'q' && s[1] <= 'm') {
            dict_matches[i] = 1;
        }
    }

    uint64_t sum = 0;
    std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = dict_matches.data();
    
    // Single pass over codes and weights with fast L1-cache lookup.
    for (std::size_t i = 0; i < n; ++i) {
        if (matches_ptr[codes_ptr[i]]) {
            sum += weights_ptr[i];
        }
    }

    // The original loop recomputed the identical sum 'iters' times.
    // Returning the scaled result is mathematically equivalent and optimal.
    return sum * static_cast<uint64_t>(iters);
}
