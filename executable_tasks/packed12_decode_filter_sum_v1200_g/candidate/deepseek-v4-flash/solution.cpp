#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
    // Pre-decode once
    const std::size_t num_values = (packed.size() / 3u) * 2u;
    std::vector<uint32_t> decoded;
    decoded.reserve(num_values);
    
    for (std::size_t i = 0; i < packed.size(); i += 3u) {
        const uint32_t a = static_cast<uint32_t>(packed[i]) |
                           (static_cast<uint32_t>(packed[i + 1u] & 0x0fu) << 8u);
        const uint32_t b = (static_cast<uint32_t>(packed[i + 1u] >> 4u)) |
                           (static_cast<uint32_t>(packed[i + 2u]) << 4u);
        decoded.push_back(a);
        decoded.push_back(b);
    }
    
    // Use raw pointer for faster access
    const uint32_t* data = decoded.data();
    const std::size_t count = decoded.size();
    const uint32_t thresh = threshold;
    
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        for (std::size_t j = 0; j < count; ++j) {
            if (data[j] > thresh) {
                local_sum += static_cast<uint64_t>(data[j]);
            }
        }
        sum = local_sum;
    }
    return sum;
}