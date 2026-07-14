#include "interface.h"
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    std::vector<uint32_t> matching_weights;
    matching_weights.reserve(codes.size());
    
    for (size_t i = 0; i < codes.size(); ++i) {
        const auto& s = dictionary[codes[i]];
        if (s[0] == 'q' && s[1] <= 'm') {
            matching_weights.push_back(weights[i]);
        }
    }
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (uint32_t w : matching_weights) {
            current_sum += w;
        }
        sum = current_sum;
    }
    return sum;
}