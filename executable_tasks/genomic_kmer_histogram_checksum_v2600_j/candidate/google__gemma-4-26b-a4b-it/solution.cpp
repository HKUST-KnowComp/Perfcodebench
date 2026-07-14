#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast FNV-1a style checksum as per the original implementation logic
inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL;
  }
  return hash;
}

// Map DNA characters to 2-bit values
inline uint32_t char_to_bits(char c) {
  switch (c) {
    case 'C': return 1u;
    case 'G': return 2u;
    case 'T': return 3u;
    default:  return 0u; // 'A' or other
  }
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t num_bins = 1u << (2 * k);
  const std::size_t n = dna.size();
  const std::size_t k_size = static_cast<std::size_t>(k);
  const uint32_t mask = (k_size >= 16) ? 0xFFFFFFFFu : (1u << (2 * k)) - 1;
  
  std::vector<uint32_t> dense(num_bins);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    if (n >= k_size) {
      uint32_t current_kmer = 0u;
      // Initialize first k-mer
      for (std::size_t i = 0; i < k_size; ++i) {
        current_kmer = (current_kmer << 2) | char_to_bits(dna[i]);
      }
      dense[current_kmer]++;

      // Slide the window
      for (std::size_t i = k_size; i < n; ++i) {
        current_kmer = ((current_kmer << 2) | char_to_bits(dna[i])) & mask;
        dense[current_kmer]++;
      }
    }

    final_hash = checksum_counts(dense);
  }

  return final_hash;
}
