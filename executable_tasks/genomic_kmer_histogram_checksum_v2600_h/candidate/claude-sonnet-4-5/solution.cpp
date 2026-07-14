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
  const size_t size = counts.size();
  
  // Process in chunks for better instruction-level parallelism
  size_t i = 0;
  for (; i + 3 < size; i += 4) {
    uint64_t h0 = hash ^ data[i];
    uint64_t h1 = h0 * 1099511628211ULL ^ data[i+1];
    uint64_t h2 = h1 * 1099511628211ULL ^ data[i+2];
    hash = h2 * 1099511628211ULL ^ data[i+3];
    hash *= 1099511628211ULL;
  }
  
  for (; i < size; ++i) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t n = dna.size();
  if (n < static_cast<size_t>(k)) return 1469598103934665603ULL;
  
  const size_t bins = 1u << (2 * k);
  const uint32_t mask = bins - 1u;
  std::vector<uint32_t> dense(bins);
  
  uint64_t hash = 0;
  const char* seq = dna.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Fast zero using memset
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));
    
    // Build initial k-mer
    uint32_t kmer = 0;
    for (int i = 0; i < k - 1; ++i) {
      kmer = (kmer << 2) | encode_base(seq[i]);
    }
    
    // Rolling window: slide through sequence
    for (size_t i = k - 1; i < n; ++i) {
      kmer = ((kmer << 2) | encode_base(seq[i])) & mask;
      dense[kmer]++;
    }
    
    hash = checksum_counts(dense);
  }
  
  return hash;
}