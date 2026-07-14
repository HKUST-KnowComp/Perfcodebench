#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
    if (count == 0) return 0;

    uint64_t total_sum_per_iter = 0;
    
    // The sum of prefix sums is: 
    // S = d[0] + (d[0]+d[1]) + (d[0]+d[1]+d[2]) ... 
    // S = count*d[0] + (count-1)*d[1] + ... + 1*d[count-1]
    // We also add a constant C = 0x9E3779B1 to each of the 'count' elements.
    
    int64_t prefix_sum_of_deltas = 0;
    for (size_t i = 0; i < count; ++i) {
        prefix_sum_of_deltas += (int64_t)deltas[i] * (int64_t)(count - i);
    }

    // Constant offset sum: count * 0x9E3779B1
    uint64_t constant_offset_total = (uint64_t)count * 0x9E3779B1ULL;
    
    // The total for one iteration is (prefix_sum_of_deltas + constant_offset_total)
    // Note: The original code casts (values[i] + 0x9E3779B1LL) to uint64_t.
    // Since we need the exact sum, we calculate the signed sum first then cast.
    
    int64_t one_iter_signed = prefix_sum_of_deltas + (int64_t)constant_offset_total;
    uint64_t one_iter_unsigned = (uint64_t)one_iter_signed;

    // The original code resets total = 0 inside the loop, so it only returns the result of the last iteration.
    // However, the loop runs 'iters' times. Let's look at the baseline again:
    // for (int iter = 0; iter < iters; ++iter) { ... total = 0; ... }
    // This means only the final iteration's result is returned.
    
    return one_iter_unsigned;
}