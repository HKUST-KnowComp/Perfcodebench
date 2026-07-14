#include "interface.h"
#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> is_match(dictionary.size(), 0);
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            is_match[i] = 1;
        }
    }

    const uint16_t* c_ptr = codes.data();
    const uint32_t* w_ptr = weights.data();
    const uint8_t* m_ptr = is_match.data();
    std::size_t n = codes.size();
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            current_sum += static_cast<uint64_t>(w_ptr[i]) * m_ptr[c_ptr[i]];
        }
        sum = current_sum;
    }
    return sum;
}