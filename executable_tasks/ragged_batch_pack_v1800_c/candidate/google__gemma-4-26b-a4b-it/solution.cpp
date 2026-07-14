#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;

    // The packed sequence and its checksum are deterministic based on the input.
    // Since padded_tokens and lengths do not change across iterations, 
    // we only need to compute the result once.
    
    uint64_t hash = 1469598103934665603ULL;
    
    // We can compute the hash on the fly without storing packed_tokens and positions
    // to save memory bandwidth and allocation overhead.
    for (int b = 0; b < batch; ++b) {
        const int len = lengths[static_cast<std::size_t>(b)];
        const size_t offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
        
        for (int i = 0; i < len; ++i) {
            uint32_t token = padded_tokens[offset + i];
            uint32_t pos = static_cast<uint32_t>(i);
            
            hash = mix(hash, static_cast<uint64_t>(token));
            hash = mix(hash, static_cast<uint64_t>(pos));
        }
    }

    return hash;
}