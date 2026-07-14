#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    // Precompute a compact match mask for the dictionary to avoid string lookups/copies
    std::vector<uint8_t> mask(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        // Matches baseline logic exactly: s[0] == 'q' && s[1] <= 'm'
        mask[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    std::size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* mask_ptr = mask.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (mask_ptr[codes_ptr[i]]) {
                current_sum += weights_ptr[i];
            }
        }
        sum = current_sum;
    }
    return sum;
}