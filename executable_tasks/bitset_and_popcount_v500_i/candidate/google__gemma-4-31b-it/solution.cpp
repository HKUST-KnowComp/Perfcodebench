#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t total_count = 0;
    size_t n = left.size();
    
    // The original code had a loop for 'iters' that reset 'count = 0' inside the loop,
    // effectively only returning the result of the last iteration. 
    // To preserve the logic of the baseline while optimizing, we perform the operation.
    // If the intention of 'iters' was to repeat the work for benchmarking, we keep it,
    // but the baseline's 'count = 0' inside the loop means only the final iteration matters.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_count = 0;
        for (size_t i = 0; i < n; ++i) {
            current_count += __builtin_popcountll(left[i] & right[i]);
        }
        total_count = current_count;
    }
    
    return total_count;
}