#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

// Optimized implementation using a direct lookup table for O(1) access.
// This assumes the dim_keys represent a dense or near-dense range.
// If the range is too large for a direct array, a hash table would be used,
// but for 'dense dimension' tasks, a direct mapping is the standard optimization.

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    
    if (dim_keys.empty()) return 0;

    // Find the range of keys to build a dense lookup table
    uint32_t max_key = 0;
    for (uint32_t k : dim_keys) {
        if (k > max_key) max_key = k;
    }

    // Create a lookup table. We use a vector to handle memory safely.
    // We initialize with 0. Note: if dim_keys are not contiguous, 
    // the lower_bound logic in the original code might have been handling 
    // non-existent keys by returning the next available key. 
    // However, in a 'dense dimension join', it is implied that keys are valid.
    // To preserve exact correctness of the original 'lower_bound' logic:
    // The original code: idx = lower_bound(probe_key). 
    // If probe_key is not in dim_keys, it picks the next larger key.
    // To perfectly replicate that, we must pre-process the dim_keys.

    std::vector<uint32_t> lookup(max_key + 1, 0);
    
    // To replicate std::lower_bound behavior for any arbitrary probe_key:
    // We need to map every possible key to the value of the smallest dim_key >= key.
    // Since the dimension is 'dense', we can fill the gaps.
    
    // 1. Fill the lookup with values from dim_keys
    // We use a temporary array to store the 'next' value for gaps.
    // Because dim_keys is sorted (implied by lower_bound usage),
    // we can propagate values backwards.
    
    // First, map existing keys
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }

    // To handle the 'lower_bound' logic: if a key is not present, 
    // it should return the value of the next available key in dim_keys.
    // We iterate backwards from max_key to 0 to fill gaps.
    uint32_t last_val = 0;
    // We need to find the value for the 'next' key. 
    // This is slightly tricky with a direct array if the probe_key > max_key.
    // But the original code would return dim_keys.end(), which is UB or out of bounds.
    // Assuming probe_keys <= max_key based on typical join constraints.
    
    // Correct way to replicate lower_bound via array:
    // We need lookup[i] to be dim_values[idx] where dim_keys[idx] is the first key >= i.
    // We can do this by iterating backwards through the sorted dim_keys.
    
    // Re-initialize lookup to handle the 'next larger' logic
    std::fill(lookup.begin(), lookup.end(), 0);
    
    // We use a pointer-based approach to fill the lookup table to match lower_bound
    // For a key 'k', lookup[k] = dim_values[index of first dim_key >= k]
    // We can populate this in O(max_key) time.
    
    // Start from the largest possible key and work downwards
    // If a key is in dim_keys, its value is dim_values[i].
    // If not, its value is the value of the next key in dim_keys.
    
    // Let's use a more robust approach for the lookup table:
    // We'll use the fact that dim_keys is sorted.
    size_t dim_idx = dim_keys.size();
    uint32_t current_val = 0;
    
    // We need to handle the case where probe_key > max_key. 
    // The original code: it = lower_bound(dim_keys.end(), probe_key) -> it = dim_keys.end().
    // idx = dim_keys.size(). dim_values[idx] is out of bounds. 
    // This implies probe_keys are always within the range of dim_keys.

    // Fill lookup table to mimic lower_bound
    // We iterate from max_key down to 0.
    // If i is a dim_key, current_val = dim_value.
    // lookup[i] = current_val.
    
    // To do this efficiently, we can use the sorted dim_keys.
    // We'll use a reverse loop.
    std::vector<uint32_t> fast_lookup(max_key + 1, 0);
    
    // We need to handle the 'next larger' logic. 
    // For any k, lookup[k] = dim_values[lower_bound(k)]
    // We can pre-calculate this by iterating through dim_keys backwards.
    
    // First, find the value for the largest possible key (which would be the last dim_key)
    // Actually, the original code's 'idx' could be dim_keys.size() if probe_key > max_key.
    // But dim_values[dim_keys.size()] is an error. 
    // We assume probe_key <= max_key.
    
    // Correct logic to fill fast_lookup to match lower_bound:
    // 1. Fill with 0
    // 2. For each i from max_key down to 0:
    //    If i is in dim_keys, current_val = dim_value[i]
    //    fast_lookup[i] = current_val
    
    // Since we don't have a map, we use the sorted dim_keys to fill.
    size_t d_ptr = dim_keys.size();
    uint32_t running_val = 0;
    
    // We need to handle the case where the probe_key is larger than any dim_key.
    // But the original code would crash. We'll assume probe_key <= max_key.
    
    // We'll use a simple array-based approach. 
    // To handle the 'lower_bound' requirement: 
    // if probe_key is 5 and dim_keys are [2, 7, 10], lower_bound(5) is 7.
    // So lookup[5] should be dim_values[index of 7].
    
    // We can achieve this by iterating backwards from max_key.
    // We'll use a temporary array to mark which indices are actual dim_keys.
    // But we can just use the sorted dim_keys directly.
    
    // Let's use a simpler, highly performant approach: 
    // Since it's a 'dense dimension', we can assume dim_keys are mostly contiguous.
    // If they aren't, the 'lower_bound' is the bottleneck. 
    // Let's build the lookup table such that lookup[i] = dim_values[lower_bound(i)].
    
    // Step 1: Fill lookup with 0
    // Step 2: For each dim_key, lookup[dim_key] = dim_value
    // Step 3: For i from max_key-1 down to 0: if lookup[i] == 0, lookup[i] = lookup[i+1]
    
    // Wait, the 'lower_bound' logic: if probe_key is 5 and dim_keys are [2, 7], 
    // lower_bound is 7. So lookup[5] = value of 7. 
    // If probe_key is 2, lower_bound is 2. So lookup[2] = value of 2.
    // If probe_key is 1, lower_bound is 2. So lookup[1] = value of 2.
    
    // Correct algorithm for lookup table:
    // 1. Initialize lookup[0...max_key] = 0
    // 2. For i = 0 to dim_keys.size() - 1: lookup[dim_keys[i]] = dim_values[i]
    // 3. For i = max_key - 1 down to 0:
    //    if (lookup[i] == 0) lookup[i] = lookup[i+1]
    // This way, if a key is missing, it takes the value of the next available key.
    
    // However, there's a catch: if the probe_key is 0 and dim_keys start at 10,
    // lookup[0] will be the value of dim_keys[0]. This matches lower_bound.
    
    // Let's implement this.
    for (size_t i = 0; i < dim_keys.size(); ++i) {
        lookup[dim_keys[i]] = dim_values[i];
    }
    
    // We need to be careful. If dim_keys[0] is 10, then lookup[0...9] should be dim_values[0].
    // The backward loop handles this: 
    // If we start from max_key and go down, we need to know the 'next' value.
    // Let's find the first dim_key.
    uint32_t first_key = dim_keys[0];
    uint32_t first_val = dim_values[0];
    
    // Fill from max_key down to first_key
    uint32_t last_seen_val = 0;
    // We need to find the value for the largest key first.
    // Actually, the backward loop should start from the largest dim_key.
    
    // Let's refine: 
    // 1. Fill lookup with 0.
    // 2. For i = 0 to dim_keys.size() - 1: lookup[dim_keys[i]] = dim_values[i]
    // 3. For i = max_key - 1 down to 0:
    //    if (lookup[i] == 0) lookup[i] = lookup[i+1]
    // This works! If lookup[i+1] is 0, it means no key exists >= i+1. 
    // But we know at least one key exists (the max_key).
    
    // One edge case: if probe_key > max_key, the original code would access dim_values[dim_keys.size()].
    // We will assume probe_key <= max_key to avoid UB.
    
    for (int i = (int)max_key - 1; i >= 0; --i) {
        if (lookup[i] == 0) {
            lookup[i] = lookup[i+1];
        }
    }
    // Special case: if the probe_key is less than the first dim_key, 
    // lower_bound returns dim_keys[0]. Our loop handles this.
    // If the probe_key is exactly a dim_key, it returns that dim_key. Our loop handles this.

    uint64_t total_sum = 0;
    const uint32_t* p_keys = probe_keys.data();
    const uint32_t* p_weights = probe_weights.data();
    const uint32_t* l_vals = lookup.data();
    const size_t n = probe_keys.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_sum = 0;
        // Unroll loop for performance
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            current_iter_sum += static_cast<uint64_t>(l_vals[p_keys[i]]) * p_weights[i];
            current_iter_sum += static_cast<uint64_t>(l_vals[p_keys[i+1]]) * p_weights[i+1];
            current_iter_sum += static_cast<uint64_t>(l_vals[p_keys[i+2]]) * p_weights[i+2];
            current_iter_sum += static_cast<uint64_t>(l_vals[p_keys[i+3]]) * p_weights[i+3];
        }
        for (; i < n; ++i) {
            current_iter_sum += static_cast<uint64_t>(l_vals[p_keys[i]]) * p_weights[i];
        }
        total_sum += current_iter_sum;
    }

    // The original code returns the sum of the LAST iteration if it was just 'sum = 0' inside the loop.
    // Wait, the original code: 
    // for (int iter = 0; iter < iters; ++iter) { sum = 0; ... sum += ... } 
    // return sum;
    // This means it returns the sum of the VERY LAST iteration only.
    // Let me re-read: 'sum = 0' is INSIDE the loop. 
    // So it's not accumulating across iterations, it's just running the loop 'iters' times.
    // The return value is the result of the last iteration.
    
    // Let's correct the loop to match the original behavior exactly.
    // The original code: 
    // uint64_t sum = 0;
    // for (int iter = 0; iter < iters; ++iter) {
    //   sum = 0;
    //   for (...) { sum += ... }
    // }
    // return sum;
    
    // This is equivalent to just running it once, but the 'iters' parameter 
    // is likely there to measure performance (warmup/averaging).
    // To be safe and match the exact return value:
    
    uint64_t final_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        final_sum = 0;
        size_t i = 0;
        for (; i + 3 < n; i += 4) {
            final_sum += static_cast<uint64_t>(l_vals[p_keys[i]]) * p_weights[i];
            final_sum += static_cast<uint64_t>(l_vals[p_keys[i+1]]) * p_weights[i+1];
            final_sum += static_cast<uint64_t>(l_vals[p_keys[i+2]]) * p_weights[i+2];
            final_sum += static_cast<uint64_t>(l_vals[p_keys[i+3]]) * p_weights[i+3];
        }
        for (; i < n; ++i) {
            final_sum += static_cast<uint64_t>(l_vals[p_keys[i]]) * p_weights[i];
        }
    }
    return final_sum;
}
