#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<uint64_t> packed(static_cast<std::size_t>(vocab));
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* base = logits.data() + static_cast<std::size_t>(b) * vocab;
            for (int v = 0; v < vocab; ++v) {
                uint32_t u_score = static_cast<uint32_t>(base[v]) ^ 0x80000000U;
                packed[v] = (static_cast<uint64_t>(u_score) << 32) | static_cast<uint32_t>(~v);
            }
            std::partial_sort(packed.begin(), packed.begin() + topk, packed.end(), [](uint64_t a, uint64_t b) {
                return a > b;
            });
            
            for (int k = 0; k < topk; ++k) {
                uint64_t val = packed[k];
                uint32_t score = static_cast<uint32_t>(val >> 32) ^ 0x80000000U;
                uint32_t idx = ~static_cast<uint32_t>(val);
                hash ^= score;
                hash *= FNV_PRIME;
                hash ^= idx;
                hash *= FNV_PRIME;
            }
        }
    }
    return hash;
}