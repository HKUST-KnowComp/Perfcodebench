#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    // Precompute which dictionary entries match the prefix condition.
    std::vector<uint8_t> dict_match(dictionary.size());
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const std::string& s = dictionary[i];
        dict_match[i] = (s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const std::size_t n = codes.size();
    const uint16_t* code_ptr = codes.data();
    const uint32_t* weight_ptr = weights.data();
    const uint8_t* match_ptr = dict_match.data();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (match_ptr[code_ptr[i]]) {
                sum += weight_ptr[i];
            }
        }
    }
    return sum;
}
