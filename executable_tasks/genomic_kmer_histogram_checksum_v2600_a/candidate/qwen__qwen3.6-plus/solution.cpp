#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t bins = 1ULL << (2 * k);
  std::vector<uint32_t> dense(bins, 0);
  uint64_t hash = 0;
  const uint32_t mask = (1ULL << (2 * k)) - 1;
  const size_t n = dna.size();
  const size_t k_sz = static_cast<size_t>(k);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0);
    uint32_t window = 0;
    for (size_t i = 0; i < n; ++i) {
      char c = dna[i];
      uint32_t val = 0;
      if (c == 'C') val = 1;
      else if (c == 'G') val = 2;
      else if (c == 'T') val = 3;
      
      window = ((window << 2) | val) & mask;
      if (i + 1 >= k_sz) {
        dense[window]++;
      }
    }
    
    uint64_t h = 1469598103934665603ULL;
    for (size_t j = 0; j < bins; ++j) {
      h ^= static_cast<uint64_t>(dense[j]);
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}