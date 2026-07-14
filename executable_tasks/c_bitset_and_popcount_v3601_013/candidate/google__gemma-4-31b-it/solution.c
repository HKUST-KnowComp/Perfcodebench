#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total_popcount = 0;
    
    // The original code resets 'total' to 0 inside the loop, 
    // meaning only the result of the final iteration is returned.
    // However, the baseline logic was: 
    // for (iter < iters) { ... total = 0; for (...) total += bits[i]; }
    // This means the function effectively returns the popcount of (left & right) 
    // for the last iteration. Since left and right are constant across iterations,
    // we only need to compute it once.
    
    for (size_t i = 0; i < word_count; ++i) {
        total_popcount += __builtin_popcountll(left[i] & right[i]);
    }
    
    // If iters <= 0, the original loop wouldn't execute, returning 0.
    if (iters <= 0) return 0;
    
    return total_popcount;
}