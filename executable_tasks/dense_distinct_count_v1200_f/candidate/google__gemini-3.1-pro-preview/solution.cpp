#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (iters <= 0) return 0;
    
    // Allocate enough 64-bit words to cover up to key_space (inclusive)
    size_t num_words = (key_space + 64) / 64;
    std::vector<uint64_t> seen(num_words, 0);
    uint64_t count = 0;
    
    // The result is invariant across iterations, so we only need to compute it once.
    for (uint32_t key : keys) {
        uint32_t word_idx = key >> 6;
        uint64_t mask = 1ULL << (key & 63);
        uint64_t word = seen[word_idx];
        
        if ((word & mask) == 0) {
            seen[word_idx] = word | mask;
            count++;
        }
    }
    
    return count;
}
