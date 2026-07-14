#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint8_t> match(dictionary.size(), 0);
    for (size_t i = 0; i < dictionary.size(); ++i) {
        const auto& s = dictionary[i];
        if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
            match[i] = 1;
        }
    }

    uint64_t sum = 0;
    const uint16_t* c_ptr = codes.data();
    const uint32_t* w_ptr = weights.data();
    const uint8_t* m_ptr = match.data();
    size_t n = codes.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cur = 0;
        for (size_t i = 0; i < n; ++i) {
            if (m_ptr[c_ptr[i]]) {
                cur += w_ptr[i];
            }
        }
        sum = cur;
    }
    return sum;
}