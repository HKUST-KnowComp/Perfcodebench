#include "interface.h"

#include <vector>
#include <cstdint>

/**
 * Optimized implementation of bitset intersection popcount.
 * 
 * The original implementation was extremely slow due to:
 * 1. Repeatedly resizing a large std::vector<uint8_t>.
 * 2. Manual bit-by-bit extraction using shifts and masks.
 * 3. A secondary pass to sum the bits.
 * 
 * This version uses the `__builtin_popcountll` intrinsic, which maps to the 
 * hardware POPCNT instruction on modern CPUs, and processes the data in-place.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  
  uint64_t count = 0;
  const std::size_t size = left.size();
  
  // Cache pointers to avoid repeated vector overhead inside the loops
  const uint64_t* l_ptr = left.data();
  const uint64_t* r_ptr = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_count = 0;
    
    // Unrolling the loop manually or relying on the compiler to use SIMD/POPCNT
    // efficiently. Most modern compilers will optimize this loop well.
    for (std::size_t i = 0; i < size; ++i) {
      current_count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
    }
    
    count = current_count;
  }
  
  return count;
}