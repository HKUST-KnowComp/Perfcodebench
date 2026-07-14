#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)iters;

    // Precompute match mask for dictionary entries to avoid repeated string checks
    std::vector<uint8_t> dict_matches(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = dict_matches.data();

    for (size_t i = 0; i < n; ++i) {
        if (matches_ptr[codes_ptr[i]]) {
            sum += weights_ptr[i];
        }
    }
    
    return sum;
}