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
    // Handle edge case where rows might be 0, though unlikely per task description
    uint64_t hash = 1469598103934665603ULL;
    // offsets would be [0]
    return mix(hash, 0);
  }

  std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset offsets
    for (uint32_t i = 0; i <= rows; ++i) {
      offsets[i] = 0;
    }

    // Since row_ids is sorted, we can build the counts/offsets in one pass
    // We first count occurrences of each row
    // Because it's sorted, we can just iterate and increment
    for (uint32_t row : row_ids) {
      if (row < rows) {
        offsets[row + 1]++;
      }
    }

    // Convert counts to prefix sums (CSR offsets)
    uint32_t current_offset = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      current_offset += offsets[r + 1]; // This is actually the count for row r
      // Wait, the logic above: offsets[r+1] currently holds the count of row r.
      // Let's fix the logic to be cleaner.
    }
    
    // Re-implementing logic for clarity and speed:
    // 1. Clear offsets
    // 2. Count row occurrences
    // 3. Prefix sum
    
    // Let's use a more direct approach:
    // offsets[i] will store the count of row i
    // Then we transform it into prefix sums.
    
    // Resetting offsets to 0
    for (size_t i = 0; i <= rows; ++i) offsets[i] = 0;
    
    for (uint32_t row : row_ids) {
      if (row < rows) {
        offsets[row]++;
      }
    }

    // Now offsets[r] = count of row r
    // We need CSR: offsets[0]=0, offsets[1]=count[0], offsets[2]=count[0]+count[1]...
    uint32_t running_sum = 0;
    offsets[0] = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      uint32_t count = offsets[r];
      running_sum += count;
      offsets[r + 1] = running_sum;
    }

    final_hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      final_hash = mix(final_hash, static_cast<uint64_t>(v));
    }
  }

  return final_hash;
}
