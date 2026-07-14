#include "interface.h"

#include <cstdint>
#include <string>
#include <cstring>

namespace {

inline uint32_t char_to_code(char c) {
  static const uint32_t lookup[256] = {
    ['A'] = 0u, ['C'] = 1u, ['G'] = 2u, ['T'] = 3u
  };
  return lookup[static_cast<unsigned char>(c)];
}

uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = bins - 1u;
  uint32_t* dense = new uint32_t[bins];
  
  uint64_t hash = 0;
  const std::size_t n = dna.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense, 0, bins * sizeof(uint32_t));
    
    if (n >= static_cast<std::size_t>(k)) {
      uint32_t code = 0u;
      for (int i = 0; i < k - 1; ++i) {
        code = (code << 2) | char_to_code(dna[i]);
      }
      
      for (std::size_t i = k - 1; i < n; ++i) {
        code = ((code << 2) | char_to_code(dna[i])) & mask;
        dense[code]++;
      }
    }
    
    hash = checksum_counts(dense, bins);
  }
  
  delete[] dense;
  return hash;
}