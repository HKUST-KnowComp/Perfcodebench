#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

// Bitset for O(1) lookup
class Bitset {
public:
    std::vector<uint64_t> data;
    
    void init(uint32_t max_val) {
        size_t num_words = (static_cast<size_t>(max_val) + 64) / 64;
        data.assign(num_words, 0);
    }
    
    void set(uint32_t val) {
        data[val >> 6] |= (1ULL << (val & 63));
    }
    
    bool test(uint32_t val) const {
        return (data[val >> 6] >> (val & 63)) & 1;
    }
};

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
    
    const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
    if (num_requests == 0) {
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
        }
        return hash;
    }
    
    // Use bitset for O(1) lookup if hash_space is reasonable (up to 256M = 32MB bitset)
    constexpr uint32_t MAX_BITSET_SIZE = 256 * 1024 * 1024;
    const bool use_bitset = hash_space <= MAX_BITSET_SIZE && hash_space > 0;
    
    Bitset cache_bitset;
    if (use_bitset) {
        cache_bitset.init(hash_space);
        for (uint32_t key : sorted_cache_keys) {
            if (key < hash_space) {
                cache_bitset.set(key);
            }
        }
    }
    
    // Precompute prefix lengths for all requests
    std::vector<uint64_t> prefix_lengths(num_requests);
    
    const uint32_t* blocks_ptr = request_blocks.data();
    const int* offsets_ptr = request_offsets.data();
    
    if (use_bitset) {
        const uint64_t* bitset_data = cache_bitset.data.data();
        
        for (size_t r = 0; r < num_requests; ++r) {
            int start = offsets_ptr[r];
            int end = offsets_ptr[r + 1];
            uint64_t prefix = 0;
            
            for (int i = start; i < end; ++i) {
                uint32_t block = blocks_ptr[i];
                uint64_t word = bitset_data[block >> 6];
                if (!((word >> (block & 63)) & 1)) {
                    break;
                }
                ++prefix;
            }
            prefix_lengths[r] = prefix;
        }
    } else {
        // Fallback to binary search for very large hash spaces
        const uint32_t* cache_begin = sorted_cache_keys.data();
        const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
        
        for (size_t r = 0; r < num_requests; ++r) {
            int start = offsets_ptr[r];
            int end = offsets_ptr[r + 1];
            uint64_t prefix = 0;
            
            for (int i = start; i < end; ++i) {
                uint32_t block = blocks_ptr[i];
                if (!std::binary_search(cache_begin, cache_end, block)) {
                    break;
                }
                ++prefix;
            }
            prefix_lengths[r] = prefix;
        }
    }
    
    // Compute hash for all iterations
    uint64_t hash = 0;
    const uint64_t* prefix_ptr = prefix_lengths.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        // Unroll hash computation
        size_t r = 0;
        for (; r + 4 <= num_requests; r += 4) {
            hash = mix(hash, prefix_ptr[r]);
            hash = mix(hash, prefix_ptr[r + 1]);
            hash = mix(hash, prefix_ptr[r + 2]);
            hash = mix(hash, prefix_ptr[r + 3]);
        }
        for (; r < num_requests; ++r) {
            hash = mix(hash, prefix_ptr[r]);
        }
    }
    
    return hash;
}