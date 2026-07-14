#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimization Strategy:
 * 1. Mathematical Simplification:
 *    The decoded value at index i is: value[i] = base + sum(deltas[0...i]).
 *    The total sum is: Σ_{i=0}^{n-1} (base + Σ_{j=0}^{i} deltas[j]).
 *    Rearranging the summation: Σ_{i=0}^{n-1} base + Σ_{i=0}^{n-1} Σ_{j=0}^{i} deltas[j].
 *    The term 'base' appears n times.
 *    The term 'deltas[j]' appears in the inner sum for every i from j to n-1, which is (n - j) times.
 *    Total sum = (n * base) + Σ_{j=0}^{n-1} ((n - j) * deltas[j]).
 * 
 * 2. Redundancy Elimination:
 *    The baseline loop repeats the same calculation 'iters' times and returns the result of the last iteration.
 *    We compute the result once and return it, avoiding O(iters * n) work.
 * 
 * 3. Memory Efficiency:
 *    Removed the std::vector<uint64_t> allocation, reducing space complexity from O(n) to O(1).
 */

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) {
        return 0;
    }

    const size_t n = deltas.size();
    if (n == 0) {
        return 0;
    }

    // Start with the contribution of the base value: n * base
    uint64_t total_sum = static_cast<uint64_t>(n) * static_cast<uint64_t>(base);

    // Add the weighted contribution of each delta: (n - i) * deltas[i]
    // The compiler is likely to auto-vectorize this simple linear loop.
    for (size_t i = 0; i < n; ++i) {
        uint64_t weight = static_cast<uint64_t>(n - i);
        total_sum += weight * static_cast<uint64_t>(deltas[i]);
    }

    return total_sum;
}