#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    const std::size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
    if (num_segments == 0) {
        // No segments, hash is just the initial value after 0 mixes
        return iters > 0 ? FNV_OFFSET : 0;
    }
    
    // Precompute segment lengths
    std::vector<uint32_t> lengths(num_segments);
    for (std::size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
    }
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        for (std::size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = lengths[s];
            const uint64_t seg_id = static_cast<uint64_t>(s);
            
            // Unroll the inner loop for better performance
            uint32_t i = 0;
            
            // Process 8 at a time
            for (; i + 8 <= len; i += 8) {
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
                hash = (hash ^ seg_id) * FNV_PRIME;
            }
            
            // Process remaining
            for (; i < len; ++i) {
                hash = (hash ^ seg_id) * FNV_PRIME;
            }
        }
    }
    
    return hash;
}