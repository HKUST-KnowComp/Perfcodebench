#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (rows == 0) {
        uint64_t hash = 1469598103934665603ULL;
        // If rows is 0, offsets is [0]
        return mix(hash, 0);
    }

    std::vector<uint32_t> offsets(rows + 1, 0);
    
    // Since row_ids are sorted, we can build the offsets in O(N + R)
    // First, count occurrences using the sorted property
    // However, a simpler way to build CSR offsets from sorted COO is:
    // 1. Initialize offsets with 0.
    // 2. For each row_id, increment the count for that row.
    // 3. Compute prefix sums.
    
    // To match the exact logic of the baseline (which counts all row_ids and then builds prefix sums):
    // We can use a frequency array since rows is likely manageable.
    
    for (int iter = 0; iter < iters; ++iter) {
        // Reset offsets
        for (uint32_t i = 0; i <= rows; ++i) {
            offsets[i] = 0;
        }

        // Count frequencies
        for (uint32_t row : row_ids) {
            if (row < rows) {
                offsets[row + 1]++;
            }
        }

        // Convert counts to prefix sums (CSR offsets)
        // The baseline logic: offsets[r+1] = offsets[r] + count[r]
        // Our offsets[row+1] currently holds the count of 'row'.
        // We need to transform this into prefix sums.
        
        // Correct logic to match baseline:
        // 1. counts[r] = number of times r appears in row_ids
        // 2. offsets[0] = 0
        // 3. offsets[r+1] = offsets[r] + counts[r]
        
        // Let's re-implement precisely to ensure correctness:
        // We'll use a temporary frequency array to avoid map overhead.
        // But we can do it in-place in 'offsets' if we are careful.
        
        // Step 1: counts[r] is stored in offsets[r+1]
        // Step 2: Prefix sum
        uint32_t current_sum = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            uint32_t count_r = offsets[r + 1];
            offsets[r + 1] = current_sum + count_r;
            current_sum = offsets[r + 1];
        }
        // The loop above actually makes offsets[r+1] = sum_{i=0}^{r} count_i
        // Wait, the baseline does: offsets[r+1] = offsets[r] + count[r]
        // Let's trace: 
        // r=0: offsets[1] = offsets[0] + count[0] = 0 + count[0]
        // r=1: offsets[2] = offsets[1] + count[1] = count[0] + count[1]
        // This is exactly what the prefix sum does.
        
        // However, the baseline loop for r=0 to rows-1 sets offsets[r+1].
        // It does NOT set offsets[0] explicitly in the loop, but offsets is initialized to 0.
        // So offsets[0] is 0.
        
        // Let's refine the prefix sum to match the baseline exactly:
        // The baseline: 
        // offsets[0] = 0
        // for r = 0 to rows-1:
        //    offsets[r+1] = offsets[r] + count[r]
        
        // My logic above:
        // current_sum = 0
        // for r = 0 to rows-1:
        //    count_r = offsets[r+1]
        //    offsets[r+1] = current_sum + count_r
        //    current_sum = offsets[r+1]
        // This is correct.

        // Re-calculating the hash
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash = mix(hash, static_cast<uint64_t>(v));
        }
        
        if (iter == iters - 1) {
            return hash;
        }
    }
    
    return 0;
}
