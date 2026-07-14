#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
    if (packed.size() < 3u) return 0;
    
    uint64_t sum = 0;
    const uint32_t thr = static_cast<uint32_t>(threshold);
    
    // Process only complete triplets to match original behaviour
    const std::size_t limit = packed.size() - (packed.size() % 3u);
    for (std::size_t i = 0; i < limit; i += 3u) {
        const uint8_t b0 = packed[i];
        const uint8_t b1 = packed[i + 1u];
        const uint8_t b2 = packed[i + 2u];
        
        const uint32_t a = static_cast<uint32_t>(b0) | (static_cast<uint32_t>(b1 & 0x0Fu) << 8u);
        if (a > thr) sum += a;
        
        const uint32_t b = (static_cast<uint32_t>(b1) >> 4u) | (static_cast<uint32_t>(b2) << 4u);
        if (b > thr) sum += b;
    }
    
    return sum;
}