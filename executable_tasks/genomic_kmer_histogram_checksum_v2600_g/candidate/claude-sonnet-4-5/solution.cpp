#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint32_t encode_base(char c) {
  // Branchless encoding: A=0, C=1, G=2, T=3
  // ASCII: A=65, C=67, G=71, T=84
  // Use bit pattern: (c >> 1) & 3 gives us a unique 2-bit value
  return (c >> 1) & 3u;
}

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* data = counts.data();
  const std::size_t size = counts.size();
  
  for (std::size_t i = 0; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t n = dna.size();
  if (n < static_cast<std::size_t>(k)) {
    return checksum_counts(std::vector<uint32_t>(1u << (2 * k), 0u));
  }
  
  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = bins - 1u;
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Zero out only on subsequent iterations
    if (iter > 0) {
      std::memset(dense.data(), 0, bins * sizeof(uint32_t));
    }
    
    // Build initial k-mer
    uint32_t kmer = 0u;
    for (int i = 0; i < k; ++i) {
      kmer = (kmer << 2) | encode_base(dna[i]);
    }
    dense[kmer]++;
    
    // Rolling window: slide through remaining positions
    const int shift = 2 * (k - 1);
    for (std::size_t i = k; i < n; ++i) {
      kmer = ((kmer << 2) | encode_base(dna[i])) & mask;
      dense[kmer]++;
    }
    
    hash = checksum_counts(dense);
  }
  
  return hash;
}
