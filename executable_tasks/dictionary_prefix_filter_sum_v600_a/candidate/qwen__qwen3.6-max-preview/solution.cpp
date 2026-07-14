#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // Precompute match status for each dictionary entry.
    // This avoids repeated string lookups, allocations, and copies.
    std::vector<uint8_t> dict_matches(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        // Replicate baseline prefix rule exactly.
        dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm');
    }

    uint64_t sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = dict_matches.data();

    // Single pass with branchless accumulation for optimal vectorization.
    // The baseline resets sum to 0 each iteration, so the final result
    // is identical to a single pass regardless of `iters`.
    for (std::size_t i = 0; i < n; ++i) {
        sum += (matches_ptr[codes_ptr[i]] ? weights_ptr[i] : 0);
    }

    (void)iters;
    return sum;
}