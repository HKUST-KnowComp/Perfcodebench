#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t bins = static_cast<std::size_t>(1ULL << (2 * k));
  const uint32_t mask = static_cast<uint32_t>((1ULL << (2 * k)) - 1);
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  const std::size_t k_sz = static_cast<std::size_t>(k);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    
    uint32_t code = 0;
    std::size_t len = 0;
    
    for (unsigned char c : dna) {
      uint32_t v = 0;
      if (c == 'C') v = 1;
      else if (c == 'G') v = 2;
      else if (c == 'T') v = 3;
      
      code = ((code << 2) & mask) | v;
      if (len < k_sz) ++len;
      if (len == k_sz) {
        dense[code]++;
      }
    }
    
    uint64_t h = 1469598103934665603ULL;
    for (uint32_t count : dense) {
      h ^= static_cast<uint64_t>(count);
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}