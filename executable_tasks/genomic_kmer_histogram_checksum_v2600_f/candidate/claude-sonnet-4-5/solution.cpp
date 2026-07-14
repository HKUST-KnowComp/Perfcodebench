#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_counts(const uint32_t* counts, size_t bins) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  size_t limit = bins & ~3ULL;
  
  for (; i < limit; i += 4) {
    uint64_t h0 = hash ^ static_cast<uint64_t>(counts[i]);
    h0 *= prime;
    h0 ^= static_cast<uint64_t>(counts[i + 1]);
    h0 *= prime;
    h0 ^= static_cast<uint64_t>(counts[i + 2]);
    h0 *= prime;
    h0 ^= static_cast<uint64_t>(counts[i + 3]);
    hash = h0 * prime;
  }
  
  for (; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= prime;
  }
  
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t bins = 1u << (2 * k);
  std::vector<uint32_t> dense(bins);
  uint64_t hash = 0;
  
  const size_t n = dna.size();
  const size_t num_kmers = n >= static_cast<size_t>(k) ? n - k + 1 : 0;
  
  if (num_kmers == 0) {
    return checksum_counts(dense.data(), bins);
  }
  
  const uint32_t mask = (1u << (2 * k)) - 1u;
  
  for (int iter = 0; iter < iters; ++iter) {
    memset(dense.data(), 0, bins * sizeof(uint32_t));
    
    uint32_t code = 0u;
    size_t valid = 0;
    
    for (size_t i = 0; i < n; ++i) {
      char c = dna[i];
      uint32_t v = 0u;
      if (c == 'C') v = 1u;
      else if (c == 'G') v = 2u;
      else if (c == 'T') v = 3u;
      
      code = ((code << 2) | v) & mask;
      ++valid;
      
      if (valid >= static_cast<size_t>(k)) {
        ++dense[code];
      }
    }
    
    hash = checksum_counts(dense.data(), bins);
  }
  
  return hash;
}
