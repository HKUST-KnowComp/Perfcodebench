#include "interface.h"
#include <cstdint>
#include <vector>

/**
 * Optimized bitset intersection popcount.
 * 
 * The baseline implementation was extremely slow due to:
 * 1. Repeated memory allocation (vector::resize inside a loop).
 * 2. Manual bit extraction and storage into a byte vector.
 * 3. Summing bytes instead of using hardware popcount instructions.
 * 
 * This version uses __builtin_popcountll, which maps to the POPCNT instruction on modern CPUs,
 * and unrolls the loop to maximize throughput.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    
    const size_t n = left.size();
    const uint64_t* l_ptr = left.data();
    const uint64_t* r_ptr = right.data();
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        size_t i = 0;

        // Unroll the loop by 4 to improve instruction-level parallelism
        for (; i + 3 < n; i += 4) {
            uint64_t c0 = l_ptr[i] & r_ptr[i];
            uint64_t c1 = l_ptr[i + 1] & r_ptr[i + 1];
            uint64_t c2 = l_ptr[i + 2] & r_ptr[i + 2];
            uint64_t c3 = l_ptr[i + 3] & r_ptr[i + 3];
            
            count += __builtin_popcountll(c0);
            count += __builtin_popcountll(c1);
            count += __builtin_popcountll(c2);
            count += __builtin_popcountll(c3);
        }

        // Handle remaining elements
        for (; i < n; ++i) {
            count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
        }
    }

    return count;
}