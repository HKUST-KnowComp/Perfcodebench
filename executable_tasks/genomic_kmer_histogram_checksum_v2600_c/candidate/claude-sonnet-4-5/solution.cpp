#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint32_t char_to_bits(char c) {
  // A=0, C=1, G=2, T=3
  return (c >> 1) & 3u;
}

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
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
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    
    if (dna.size() < static_cast<std::size_t>(k)) {
      hash = checksum_counts(dense);
      continue;
    }
    
    // Build initial k-mer
    uint32_t kmer = 0u;
    for (int i = 0; i < k; ++i) {
      kmer = (kmer << 2) | char_to_bits(dna[i]);
    }
    dense[kmer]++;
    
    // Rolling hash for remaining k-mers
    const int shift = 2 * (k - 1);
    for (std::size_t i = k; i < dna.size(); ++i) {
      kmer = ((kmer << 2) | char_to_bits(dna[i])) & mask;
      dense[kmer]++;
    }
    
    hash = checksum_counts(dense);
  }
  return hash;
}