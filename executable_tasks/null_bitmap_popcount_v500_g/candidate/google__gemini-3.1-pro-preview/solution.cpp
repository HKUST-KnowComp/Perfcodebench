#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimized bitmap popcount.
 * 
 * The original implementation expanded each bit into a std::vector<uint8_t> and summed them,
 * repeating this process 'iters' times. Since the input 'words' does not change and the 
 * result of the last iteration is returned, we can compute the popcount once using 
 * the compiler intrinsic __builtin_popcountll, which maps to efficient CPU instructions 
 * (like POPCNT on x86).
 */
uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  // If iters is 0 or less, the loop in the original code never executes,
  // and the initial count of 0 is returned.
  if (iters <= 0) {
    return 0;
  }

  uint64_t total_count = 0;
  
  // Calculate the popcount of the entire vector.
  // We use __builtin_popcountll for high performance.
  for (uint64_t word : words) {
    total_count += static_cast<uint64_t>(__builtin_popcountll(word));
  }

  // The original code resets 'count' to 0 at the start of every iteration's 
  // summation loop. Thus, the final value returned is simply the popcount 
  // of the words vector calculated in the final iteration.
  return total_count;
}