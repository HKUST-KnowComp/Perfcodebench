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
    
    // Precompute match table: O(dict_size) once, fits easily in L1/L2 cache
    std::vector<uint8_t> dict_matches(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        dict_matches[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    size_t n = codes.size();
    const uint16_t* codes_ptr = codes.data();
    const uint32_t* weights_ptr = weights.data();
    const uint8_t* matches_ptr = dict_matches.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            iter_sum += matches_ptr[codes_ptr[i]] ? weights_ptr[i] : 0;
        }
        sum = iter_sum;
    }
    return sum;
}
