#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    
    // Use stack for small key spaces, heap for large
    constexpr size_t STACK_LIMIT = 1 << 17; // 128K words = 1MB stack
    
    uint64_t count = 0;
    
    if (num_words <= STACK_LIMIT) {
        alignas(64) uint64_t bitmap[STACK_LIMIT];
        
        for (int iter = 0; iter < iters; ++iter) {
            std::memset(bitmap, 0, num_words * sizeof(uint64_t));
            
            for (uint32_t key : keys) {
                bitmap[key >> 6] |= (1ULL << (key & 63));
            }
            
            count = 0;
            for (size_t i = 0; i < num_words; ++i) {
                count += __builtin_popcountll(bitmap[i]);
            }
        }
    } else {
        std::vector<uint64_t> bitmap(num_words);
        
        for (int iter = 0; iter < iters; ++iter) {
            std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
            
            for (uint32_t key : keys) {
                bitmap[key >> 6] |= (1ULL << (key & 63));
            }
            
            count = 0;
            for (size_t i = 0; i < num_words; ++i) {
                count += __builtin_popcountll(bitmap[i]);
            }
        }
    }
    
    return count;
}