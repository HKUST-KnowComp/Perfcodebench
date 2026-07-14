#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

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
  // Compile-time base 2-bit encoding lookup table
  static const uint8_t base_map[256] = []() consteval {
    uint8_t arr[256] = {0};
    arr['A'] = 0; arr['C'] = 1; arr['G'] = 2; arr['T'] = 3;
    return arr;
  }();

  const std::size_t n = dna.size();
  const std::size_t k_sz = static_cast<std::size_t>(k);
  const std::size_t bins = 1ULL << (2 * k);
  const uint32_t mask = (1U << (2 * k)) - 1;

  // Precompute base codes once for all iterations
  std::vector<uint8_t> dna_codes(n);
  for (std::size_t i = 0; i < n; ++i) {
    dna_codes[i] = base_map[static_cast<unsigned char>(dna[i])];
  }

  std::vector<uint32_t> dense(bins, 0u);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    if (n < k_sz) {
      final_hash = checksum_counts(dense);
      continue;
    }

    // Compute first k-mer code
    uint32_t current_code = 0;
    for (std::size_t i = 0; i < k_sz; ++i) {
      current_code = (current_code << 2) | dna_codes[i];
    }
    dense[current_code]++;

    // Roll the k-mer window across the rest of the sequence
    for (std::size_t i = 1; i <= n - k_sz; ++i) {
      current_code = (current_code << 2) & mask;
      current_code |= dna_codes[i + k_sz - 1];
      dense[current_code]++;
    }

    final_hash = checksum_counts(dense);
  }

  return final_hash;
}