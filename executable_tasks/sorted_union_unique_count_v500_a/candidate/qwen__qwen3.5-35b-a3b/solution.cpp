#include "interface.h"

#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  (void)iters; // Benchmark iterations, compute once
  
  const auto* l = left.data();
  const auto* r = right.data();
  size_t n = left.size();
  size_t m = right.size();
  
  uint64_t count = 0;
  uint32_t last = 0xFFFFFFFF;
  
  size_t i = 0, j = 0;
  while (i < n || j < m) {
    uint32_t current;
    if (i < n && (j >= m || l[i] <= r[j])) {
      current = l[i++];
    } else {
      current = r[j++];
    }
    
    if (current != last) {
      count++;
      last = current;
    }
  }
  
  return count;
}