#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
  std::vector<uint32_t> a = input;
  std::vector<uint32_t> b(a.size());
  
  const size_t C = static_cast<size_t>(cols);
  const size_t R = static_cast<size_t>(rows);
  
  for (int step = 0; step < steps; ++step) {
    uint32_t* pa = a.data();
    uint32_t* pb = b.data();
    
    // Copy top and bottom boundary rows
    for (size_t c = 0; c < C; ++c) {
      pb[c] = pa[c];
      pb[(R - 1) * C + c] = pa[(R - 1) * C + c];
    }
    
    #pragma omp parallel for schedule(static)
    for (size_t r = 1; r < R - 1; ++r) {
      size_t base = r * C;
      pb[base] = pa[base];
      pb[base + C - 1] = pa[base + C - 1];
      for (size_t c = 1; c < C - 1; ++c) {
        size_t idx = base + c;
        pb[idx] = (pa[idx] * 4u + pa[idx - 1] + pa[idx + 1] + pa[idx - C] + pa[idx + C]) >> 3;
      }
    }
    
    a.swap(b);
  }
  
  return checksum_u32(a);
}
