#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  uint64_t total_count = 0;
  const size_t size = left.size();
  
  // The problem asks for the count of set bits in the AND of two bitsets.
  // The baseline implementation resets 'count' inside the loop and returns the result of the last iteration.
  // However, the baseline logic is: count = 0; for(iter...){ count = 0; ...; count += bit; }
  // This means only the result of the final iteration is returned.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (size_t i = 0; i < size; ++i) {
      current_iter_count += __builtin_popcountll(left[i] & right[i]);
    }
    total_count = current_iter_count;
  }
  
  return total_count;
}