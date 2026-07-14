#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t n = dna.size();
  if (static_cast<std::size_t>(k) > n) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense.data(), bins);
    }
    return hash;
  }

  // Precompute base encoding lookup table
  alignas(64) uint8_t base_to_code[256];
  std::memset(base_to_code, 0, sizeof(base_to_code));
  base_to_code['A'] = 0;
  base_to_code['C'] = 1;
  base_to_code['G'] = 2;
  base_to_code['T'] = 3;

  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);
  
  std::vector<uint32_t> dense(bins, 0u);
  
  // Pre-encode the entire DNA string for faster access
  std::vector<uint8_t> encoded(n);
  const char* dna_ptr = dna.data();
  for (std::size_t i = 0; i < n; ++i) {
    encoded[i] = base_to_code[static_cast<unsigned char>(dna_ptr[i])];
  }

  uint64_t hash = 0;
  const std::size_t num_kmers = n - static_cast<std::size_t>(k) + 1;
  const uint8_t* enc_ptr = encoded.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));

    // Compute first k-mer hash
    uint32_t kmer_code = 0;
    for (int j = 0; j < k; ++j) {
      kmer_code = (kmer_code << 2) | enc_ptr[j];
    }
    dense[kmer_code]++;

    // Rolling hash for remaining k-mers
    for (std::size_t i = 1; i < num_kmers; ++i) {
      kmer_code = ((kmer_code << 2) | enc_ptr[i + k - 1]) & mask;
      dense[kmer_code]++;
    }

    hash = checksum_counts(dense.data(), bins);
  }
  return hash;
}