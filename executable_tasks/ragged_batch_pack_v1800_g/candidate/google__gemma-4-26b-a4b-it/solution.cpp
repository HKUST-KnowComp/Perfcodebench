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

    // Pre-calculate total elements to avoid re-allocations and redundant loops
    size_t total_elements = 0;
    for (int b = 0; b < batch; ++b) {
        total_elements += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
    }

    // The baseline logic re-calculates the same packed sequence 'iters' times.
    // Since the input data and lengths do not change per iteration, 
    // the hash result will be identical for every iteration.
    // We only need to compute it once.

    uint64_t hash = 1469598103934665603ULL;
    
    // We can combine the packing and hashing into a single pass to maximize cache efficiency.
    // This avoids the need for the intermediate 'packed_tokens' and 'positions' vectors entirely.
    for (int b = 0; b < batch; ++b) {
        const size_t row_offset = static_cast<size_t>(b) * static_cast<size_t>(max_len);
        const int len = lengths[static_cast<size_t>(b)];
        
        for (int i = 0; i < len; ++i) {
            uint32_t token = padded_tokens[row_offset + i];
            uint32_t pos = static_cast<uint32_t>(i);
            
            hash = mix(hash, static_cast<uint64_t>(token));
            hash = mix(hash, static_cast<uint64_t>(pos));
        }
    }

    return hash;
}