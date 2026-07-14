#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
    
    if (keys.empty()) return 0;

    // Find the range of keys to build a direct lookup table.
    // Since the task implies dense remapping, we assume keys are a contiguous or near-contiguous range.
    // To handle the 'lower_bound' logic from the baseline, we find the max key.
    uint32_t max_key = keys.back();
    
    // Pre-compute a lookup table for O(1) access.
    // We use a vector where lookup_table[key] = values[index_of_key].
    // The baseline used std::lower_bound, which effectively maps a probe to the smallest key >= probe.
    // However, in a dense remapping context, it usually implies keys[i] is the ID.
    // To preserve exact correctness of the baseline's lower_bound behavior:
    std::vector<uint32_t> lookup_table(max_key + 1);
    
    // The baseline: it = lower_bound(keys, probe); idx = it - keys.begin();
    // This means if probe is not in keys, it finds the next largest key.
    // To replicate this exactly with O(1), we fill the table such that 
    // lookup_table[p] = values[index of first key >= p].
    
    // Initialize with the value corresponding to the last key (for probes > max_key, 
    // though the baseline would return keys.end() which is UB/out of bounds if not careful).
    // Looking at the baseline: idx = it - keys.begin(). If it == keys.end(), idx == keys.size().
    // This would cause values[idx] to be out of bounds. We assume probes are within valid range.
    
    // Correctly replicate lower_bound behavior for all possible probe values up to max_key:
    // We iterate backwards to propagate the 'next' key's value to smaller indices.
    uint32_t current_val_idx = keys.size() - 1;
    for (int32_t i = static_cast<int32_t>(max_key); i >= 0; --i) {
        while (current_val_idx > 0 && keys[current_val_idx] > (uint32_t)i) {
            // This is slightly complex. Let's use a simpler approach:
            // The baseline finds the first key >= probe.
        }
    }

    // Re-evaluating: The most efficient way to replicate lower_bound for all i in [0, max_key]
    // is to fill the table using the keys.
    std::vector<uint32_t> fast_map(max_key + 1);
    size_t k_idx = 0;
    for (uint32_t i = 0; i <= max_key; ++i) {
        while (k_idx + 1 < keys.size() && keys[k_idx] < i) {
            k_idx++;
        }
        // If keys[k_idx] < i, then lower_bound would return keys.end().
        // But the baseline uses values[idx]. We must ensure idx < values.size().
        if (k_idx < keys.size() && keys[k_idx] >= i) {
            fast_map[i] = values[k_idx];
        } else {
            // This case handles probes > max_key if they exist, 
            // though the baseline would crash if idx == keys.size().
            // We'll assume probes are within a safe range or match the baseline's behavior.
            fast_map[i] = values[keys.size() - 1]; 
        }
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint32_t probe : probes) {
            // Direct lookup
            uint32_t val = (probe <= max_key) ? fast_map[probe] : values[keys.size()-1];
            hash = mix(hash, val);
        }
    }
    return hash;
}