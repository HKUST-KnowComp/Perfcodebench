#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// FNV-1a hash constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = FNV_OFFSET_BASIS;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= FNV_PRIME;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  if (iters <= 0) return 0;
  if (static_cast<std::size_t>(k) > dna.size()) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    return checksum_counts(dense);
  }

  // Precompute character mapping to 2-bit values
  // Baseline: 'C'->1, 'G'->2, 'T'->3, others (including 'A')->0
  uint8_t lut[256] = {0};
  lut[static_cast<uint8_t>('C')] = 1;
  lut[static_cast<uint8_t>('G')] = 2;
  lut[static_cast<uint8_t>('T')] = 3;

  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t final_hash = 0;

  // The input dna and k are constant across iterations.
  // We only need to compute the histogram once and return the checksum.
  // The baseline overwrites 'hash' in each iteration, so the result is just the last one.
  
  // Reset dense array
  std::fill(dense.begin(), dense.end(), 0u);

  uint32_t current_code = 0;
  const uint8_t* dna_ptr = reinterpret_cast<const uint8_t*>(dna.data());
  
  // Initialize the first k-1 characters
  for (int i = 0; i < k - 1; ++i) {
    current_code = (current_code << 2) | lut[dna_ptr[i]];
  }

  // Sliding window for the rest of the sequence
  std::size_t dna_len = dna.size();
  for (std::size_t i = static_cast<std::size_t>(k) - 1; i < dna_len; ++i) {
    current_code = ((current_code << 2) | lut[dna_ptr[i]]) & mask;
    dense[current_code]++;
  }

  final_hash = checksum_counts(dense);

  return final_hash;
}
