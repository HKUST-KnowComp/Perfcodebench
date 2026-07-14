#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    // Pre-compute total packed length
    std::size_t total_len = 0;
    for (int b = 0; b < batch; ++b) {
        total_len += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
    }
    
    // Pre-allocate once
    std::vector<uint32_t> packed_tokens(total_len);
    std::vector<uint32_t> positions(total_len);
    
    const uint32_t* __restrict__ src = padded_tokens.data();
    const int* __restrict__ lens = lengths.data();
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint32_t* __restrict__ dst_tok = packed_tokens.data();
        uint32_t* __restrict__ dst_pos = positions.data();
        
        std::size_t out_idx = 0;
        for (int b = 0; b < batch; ++b) {
            const int len = lens[b];
            const uint32_t* __restrict__ row = src + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
            for (int i = 0; i < len; ++i) {
                dst_tok[out_idx] = row[i];
                dst_pos[out_idx] = static_cast<uint32_t>(i);
                ++out_idx;
            }
        }
        
        // Compute hash
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < total_len; ++i) {
            hash ^= dst_tok[i];
            hash *= 1099511628211ULL;
            hash ^= dst_pos[i];
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}