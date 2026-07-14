#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline uint32_t char_code(char c) {
  if (c == 'C') return 1;
  if (c == 'G') return 2;
  if (c == 'T') return 3;
  return 0;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t bins = size_t(1) << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins) - 1;
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  
  const char* data = dna.data();
  const size_t len = dna.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    
    if (len >= static_cast<size_t>(k) && k > 0) {
      uint32_t code = 0;
      for (int i = 0; i < k - 1; ++i) {
        code = (code << 2) | char_code(data[i]);
      }
      for (size_t i = static_cast<size_t>(k) - 1; i < len; ++i) {
        code = ((code << 2) & mask) | char_code(data[i]);
        dense[code]++;
      }
    }
    hash = checksum_counts(dense);
  }
  return hash;
}
