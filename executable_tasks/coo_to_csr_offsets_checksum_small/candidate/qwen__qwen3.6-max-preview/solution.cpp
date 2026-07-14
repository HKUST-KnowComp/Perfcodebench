#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  std::vector<uint32_t> offsets(rows + 1);
  uint32_t* off_data = offsets.data();
  const uint32_t* ids_data = row_ids.data();
  const size_t ids_size = row_ids.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(off_data, off_data + rows + 1, 0u);
    
    for (size_t i = 0; i < ids_size; ++i) {
      off_data[ids_data[i] + 1]++;
    }
    
    uint32_t current = 0;
    for (uint32_t r = 0; r < rows; ++r) {
      current += off_data[r + 1];
      off_data[r + 1] = current;
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t r = 0; r <= rows; ++r) {
      hash ^= off_data[r];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}