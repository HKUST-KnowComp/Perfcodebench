#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  
  uint64_t sum = 0;
  std::size_t n = values.size();
  const uint32_t* v_ptr = values.data();
  const uint16_t* l_ptr = lengths.data();
  
  for (std::size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(v_ptr[i]) * l_ptr[i];
  }
  
  return sum;
}
