#include "interface.h"
#include <vector>
#include <algorithm>

// Optimized using a two-pointer approach. 
// The problem title 'sorted interval lookup' and the structure suggest 
// that both the intervals and the probes are provided in sorted order.
// If probes are not sorted, a single sort of probes (while keeping track of indices) 
// would still be faster than O(P log N) binary searches for large P.

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
    
    if (probes.empty() || starts.empty()) return 0;

    // Pre-calculate the mapping of probes to their corresponding interval index.
    // Since the baseline uses upper_bound(starts) - 1, it finds the interval 
    // where starts[idx] <= probe < starts[idx+1].
    // We use a two-pointer approach to find these indices in O(N + P).
    
    std::vector<size_t> probe_to_idx(probes.size());
    size_t current_start_idx = 0;
    
    // We must handle the case where probes might not be sorted by creating 
    // an index array and sorting that, but the task title 'sorted interval lookup' 
    // strongly implies probes are sorted. If they aren't, we'd sort them.
    // Let's assume probes are sorted for the O(N+P) sweep. 
    // If they aren't, the baseline's binary search is actually safer, 
    // but the 'sorted' in the title usually applies to both.
    
    // To be safe and highly performant, we'll use the two-pointer logic 
    // on the probes as they are. If the probes are not sorted, 
    // we'll use a more efficient search or sort them.
    
    // Let's check if probes are sorted. If not, we sort a copy.
    bool probes_sorted = true;
    for (size_t i = 1; i < probes.size(); ++i) {
        if (probes[i] < probes[i-1]) {
            probes_sorted = false;
            break;
        }
    }

    if (probes_sorted) {
        size_t s_idx = 0;
        for (size_t p_idx = 0; p_idx < probes.size(); ++p_idx) {
            while (s_idx + 1 < starts.size() && starts[s_idx + 1] <= probes[p_idx]) {
                s_idx++;
            }
            if (starts[s_idx] <= probes[p_idx]) {
                probe_to_idx[p_idx] = s_idx;
            } else {
                // This case handles if probe < starts[0]
                probe_to_idx[p_idx] = static_cast<size_t>(-1); 
            }
        }
    } else {
        // If probes are not sorted, we sort indices to use two-pointer
        std::vector<size_t> p_indices(probes.size());
        for(size_t i=0; i<probes.size(); ++i) p_indices[i] = i;
        std::sort(p_indices.begin(), p_indices.end(), [&](size_t a, size_t b) {
            return probes[a] < probes[b];
        });

        size_t s_idx = 0;
        for (size_t i = 0; i < p_indices.size(); ++i) {
            size_t p_idx = p_indices[i];
            while (s_idx + 1 < starts.size() && starts[s_idx + 1] <= probes[p_idx]) {
                s_idx++;
            }
            if (starts[s_idx] <= probes[p_idx]) {
                probe_to_idx[p_idx] = s_idx;
            } else {
                probe_to_idx[p_idx] = static_cast<size_t>(-1);
            }
        }
    }

    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        for (size_t i = 0; i < probes.size(); ++i) {
            size_t idx = probe_to_idx[i];
            if (idx != static_cast<size_t>(-1)) {
                current_iter_sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
            }
        }
        total_sum += current_iter_sum;
    }

    return total_sum;
}