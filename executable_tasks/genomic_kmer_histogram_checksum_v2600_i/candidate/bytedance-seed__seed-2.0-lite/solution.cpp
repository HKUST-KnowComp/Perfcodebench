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
  const std::size_t bins = 1u << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);
  const uint32_t mask = (1u << (2 * k)) - 1;
  
  uint8_t base_code[256] = {0};
  base_code['C'] = 1;
  base_code['G'] = 2;
  base_code['T'] = 3;

  const std::size_t dna_len = dna.size();
  const std::size_t k_sz = static_cast<std::size_t>(k);
  const std::size_t max_i = (dna_len >= k_sz) ? (dna_len - k_sz) : 0;
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    if (dna_len < k_sz) {
      final_hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0;
    for (int j = 0; j < k; ++j) {
      code = (code << 2) | base_code[static_cast<unsigned char>(dna[j])];
    }
    dense[code] += 1;

    for (std::size_t i = 1; i <= max_i; ++i) {
      const char next_base = dna[i + k - 1];
      code = (code << 2) | base_code[static_cast<unsigned char>(next_base)];
      code &= mask;
      dense[code] += 1;
    }

    final_hash = checksum_counts(dense);
  }
  return final_hash;
}