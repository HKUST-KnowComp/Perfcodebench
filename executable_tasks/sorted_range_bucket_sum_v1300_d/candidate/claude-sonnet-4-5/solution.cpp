#include "interface.h"

#include <algorithm>

static inline size_t binary_search_upper_bound(const uint32_t* data, size_t n, uint32_t value) {
    size_t left = 0;
    size_t right = n;
    
    while (right - left > 8) {
        size_t mid = left + (right - left) / 2;
        if (data[mid] <= value) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    // Linear search for small ranges
    while (left < right && data[left] <= value) {
        ++left;
    }
    
    return left;
}

static inline size_t galloping_search(const uint32_t* data, size_t n, uint32_t value, size_t hint) {
    if (hint >= n || data[hint] > value) {
        // Search backwards
        if (hint == 0 || data[hint - 1] <= value) {
            return hint;
        }
        size_t step = 1;
        size_t pos = hint;
        while (pos >= step && data[pos - step] > value) {
            pos -= step;
            step *= 2;
        }
        size_t left = (pos >= step) ? pos - step : 0;
        return binary_search_upper_bound(data + left, hint - left, value) + left;
    } else {
        // Search forwards
        size_t step = 1;
        size_t pos = hint;
        while (pos + step < n && data[pos + step] <= value) {
            pos += step;
            step *= 2;
        }
        size_t right = (pos + step < n) ? pos + step : n;
        return binary_search_upper_bound(data + hint, right - hint, value) + hint;
    }
}

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    const size_t n_boundaries = boundaries.size();
    const size_t n_probes = probes.size();
    const uint32_t* bound_data = boundaries.data();
    const uint32_t* probe_data = probes.data();
    
    uint64_t sum = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t last_idx = 0;
        
        for (size_t i = 0; i < n_probes; ++i) {
            uint32_t probe = probe_data[i];
            
            // Prefetch next probe
            if (i + 1 < n_probes) {
                __builtin_prefetch(&probe_data[i + 1], 0, 3);
            }
            
            // Use galloping search with hint from last position
            size_t idx = galloping_search(bound_data, n_boundaries, probe, last_idx);
            sum += idx;
            last_idx = idx;
        }
    }
    
    return sum;
}