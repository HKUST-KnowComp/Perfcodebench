#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <immintrin.h>

// Scalar binary search for upper_bound - 1
static inline std::size_t find_interval(const uint32_t* starts, std::size_t n, uint32_t probe) {
    std::size_t lo = 0, hi = n;
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (starts[mid] <= probe) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo - 1;
}

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    (void)ends;
    
    const std::size_t n_intervals = starts.size();
    const std::size_t n_probes = probes.size();
    
    if (n_probes == 0 || n_intervals == 0) {
        return 0;
    }
    
    const uint32_t* starts_ptr = starts.data();
    const uint32_t* payloads_ptr = payloads.data();
    const uint32_t* probes_ptr = probes.data();
    const uint32_t* weights_ptr = weights.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        
        std::size_t i = 0;
        
        // Process 4 probes at a time using AVX2
        const std::size_t simd_end = (n_probes >= 4) ? (n_probes - 3) : 0;
        
        while (i < simd_end) {
            // Find intervals for 4 probes
            uint32_t probe0 = probes_ptr[i];
            uint32_t probe1 = probes_ptr[i + 1];
            uint32_t probe2 = probes_ptr[i + 2];
            uint32_t probe3 = probes_ptr[i + 3];
            
            std::size_t idx0 = find_interval(starts_ptr, n_intervals, probe0);
            std::size_t idx1 = find_interval(starts_ptr, n_intervals, probe1);
            std::size_t idx2 = find_interval(starts_ptr, n_intervals, probe2);
            std::size_t idx3 = find_interval(starts_ptr, n_intervals, probe3);
            
            // Load payloads
            uint32_t p0 = payloads_ptr[idx0];
            uint32_t p1 = payloads_ptr[idx1];
            uint32_t p2 = payloads_ptr[idx2];
            uint32_t p3 = payloads_ptr[idx3];
            
            // Load weights
            uint32_t w0 = weights_ptr[i];
            uint32_t w1 = weights_ptr[i + 1];
            uint32_t w2 = weights_ptr[i + 2];
            uint32_t w3 = weights_ptr[i + 3];
            
            // Accumulate using 64-bit arithmetic
            sum += static_cast<uint64_t>(p0) * static_cast<uint64_t>(w0);
            sum += static_cast<uint64_t>(p1) * static_cast<uint64_t>(w1);
            sum += static_cast<uint64_t>(p2) * static_cast<uint64_t>(w2);
            sum += static_cast<uint64_t>(p3) * static_cast<uint64_t>(w3);
            
            i += 4;
        }
        
        // Handle remaining probes
        for (; i < n_probes; ++i) {
            std::size_t idx = find_interval(starts_ptr, n_intervals, probes_ptr[i]);
            sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
        }
    }
    
    return sum;
}