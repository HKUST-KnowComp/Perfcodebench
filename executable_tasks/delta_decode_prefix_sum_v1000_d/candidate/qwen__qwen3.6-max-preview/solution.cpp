#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    if (iters <= 0) return 0;
    
    std::size_t n = deltas.size();
    uint64_t sum_deltas = 0;
    const uint32_t* data = deltas.data();
    
    // Mathematical transformation of the prefix sum aggregation:
    // Sum(V_i) = N * base + Sum(deltas[j] * (N - j))
    // This formulation removes cross-iteration dependencies, allowing
    // the compiler to fully auto-vectorize the loop for maximum throughput.
    for (std::size_t i = 0; i < n; ++i) {
        sum_deltas += static_cast<uint64_t>(data[i]) * (n - i);
    }
    
    return static_cast<uint64_t>(n) * base + sum_deltas;
}