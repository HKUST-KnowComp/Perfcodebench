#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_counts(const uint32_t* counts, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline uint32_t char_to_bits(char c) {
    if (c == 'C') return 1;
    if (c == 'G') return 2;
    if (c == 'T') return 3;
    return 0;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const uint32_t bins = 1u << (2 * k);
  const uint32_t mask = bins - 1;
  
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  
  const char* data = dna.data();
  size_t n = dna.size();
  size_t min_idx = (k > 0) ? static_cast<size_t>(k) - 1 : 0;
  uint32_t* dense_ptr = dense.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense_ptr, 0, bins * sizeof(uint32_t));
    uint32_t code = 0;
    for (size_t i = 0; i < n; ++i) {
      code = ((code << 2) & mask) | char_to_bits(data[i]);
      if (i >= min_idx) {
        dense_ptr[code]++;
      }
    }
    hash = checksum_counts(dense_ptr, bins);
  }
  return hash;
}