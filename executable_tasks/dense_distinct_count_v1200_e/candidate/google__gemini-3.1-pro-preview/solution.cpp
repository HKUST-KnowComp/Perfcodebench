#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    (void)key_space;
    if (keys.empty() || iters <= 0) return 0;
    
    uint32_t max_key = keys[0];
    for (size_t i = 1; i < keys.size(); ++i) {
        if (keys[i] > max_key) max_key = keys[i];
    }
    
    size_t num_words = (max_key / 64) + 1;
    std::vector<uint64_t> bitset(num_words, 0);
    
    uint64_t count = 0;
    const uint32_t* p = keys.data();
    size_t n = keys.size();
    uint64_t* b = bitset.data();
    
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(b, 0, num_words * sizeof(uint64_t));
        
        size_t i = 0;
        for (; i + 7 < n; i += 8) {
            uint32_t k0 = p[i];
            uint32_t k1 = p[i+1];
            uint32_t k2 = p[i+2];
            uint32_t k3 = p[i+3];
            uint32_t k4 = p[i+4];
            uint32_t k5 = p[i+5];
            uint32_t k6 = p[i+6];
            uint32_t k7 = p[i+7];
            b[k0 >> 6] |= (1ULL << (k0 & 63));
            b[k1 >> 6] |= (1ULL << (k1 & 63));
            b[k2 >> 6] |= (1ULL << (k2 & 63));
            b[k3 >> 6] |= (1ULL << (k3 & 63));
            b[k4 >> 6] |= (1ULL << (k4 & 63));
            b[k5 >> 6] |= (1ULL << (k5 & 63));
            b[k6 >> 6] |= (1ULL << (k6 & 63));
            b[k7 >> 6] |= (1ULL << (k7 & 63));
        }
        for (; i < n; ++i) {
            uint32_t key = p[i];
            b[key >> 6] |= (1ULL << (key & 63));
        }
        
        count = 0;
        for (size_t j = 0; j < num_words; ++j) {
            count += __builtin_popcountll(b[j]);
        }
    }
    
    return count;
}
