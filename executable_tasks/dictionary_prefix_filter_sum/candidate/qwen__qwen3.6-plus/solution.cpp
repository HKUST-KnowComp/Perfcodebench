#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> is_match(dictionary.size());
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        is_match[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t sum = 0;
    const size_t n = codes.size();
    const uint16_t* code_ptr = codes.data();
    const uint32_t* weight_ptr = weights.data();
    const uint8_t* match_ptr = is_match.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            if (match_ptr[code_ptr[i]]) {
                current_sum += weight_ptr[i];
            }
        }
        sum = current_sum;
    }
    return sum;
}
