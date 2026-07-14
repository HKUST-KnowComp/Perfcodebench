#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

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
  std::size_t size = counts.size();
  
  // Unroll by 4 for better throughput
  std::size_t i = 0;
  for (; i + 3 < size; i += 4) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 1]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 2]);
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(data[i + 3]);
    hash *= 1099511628211ULL;
  }
  
  // Handle remainder
  for (; i < size; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
    hash *= 1099511628211ULL;
  }
  
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = bins - 1u;
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  
  const std::size_t n = dna.size();
  if (n < static_cast<std::size_t>(k)) {
    return checksum_counts(dense);
  }
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));
    
    // Encode first k-mer
    uint32_t kmer = 0u;
    for (int i = 0; i < k; ++i) {
      kmer = (kmer << 2) | encode_base(dna[i]);
    }
    dense[kmer]++;
    
    // Rolling hash for remaining k-mers
    for (std::size_t i = k; i < n; ++i) {
      kmer = ((kmer << 2) | encode_base(dna[i])) & mask;
      dense[kmer]++;
    }
    
    hash = checksum_counts(dense);
  }
  
  return hash;
}