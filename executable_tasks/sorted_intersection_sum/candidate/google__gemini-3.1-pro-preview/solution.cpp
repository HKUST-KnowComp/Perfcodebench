#include "interface.h"

uint64_t sorted_intersection_sum(const std::vector<uint32_t>& lhs, const std::vector<uint32_t>& rhs, int iters) {
  uint64_t final_sum = 0;
  if (lhs.empty() || rhs.empty()) return 0;
  
  const size_t n = lhs.size();
  const size_t m = rhs.size();
  const uint32_t* l_ptr = lhs.data();
  const uint32_t* r_ptr = rhs.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;
    size_t j = 0;
    
    while (i < n && j < m) {
      uint32_t l = l_ptr[i];
      uint32_t r = r_ptr[j];
      sum += (l == r) ? static_cast<uint64_t>(l) : 0;
      i += static_cast<size_t>(l <= r);
      j += static_cast<size_t>(l >= r);
    }
    final_sum = sum;
  }
  return final_sum;
}
