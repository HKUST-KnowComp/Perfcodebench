#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

static const uint8_t base_map[256] = {
    ['A'] = 0,
    ['C'] = 1,
    ['G'] = 2,
    ['T'] = 3
};

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t num_bins = 1ULL << (2 * k);
  const uint32_t code_mask = static_cast<uint32_t>(num_bins - 1);
  std::vector<uint32_t> dense(num_bins, 0U);
  uint64_t final_hash = 0;
  const size_t dna_len = dna.size();
  const size_t min_dna_len = static_cast<size_t>(k);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0U);
    if (dna_len < min_dna_len) {
      final_hash = checksum_counts(dense);
      continue;
    }

    uint32_t current_code = 0U;
    for (int i = 0; i < k; ++i) {
      current_code = (current_code << 2) | base_map[static_cast<unsigned char>(dna[i])];
    }
    dense[current_code]++;

    for (size_t i = k; i < dna_len; ++i) {
      current_code = ((current_code << 2) | base_map[static_cast<unsigned char>(dna[i])]) & code_mask;
      dense[current_code]++;
    }

    final_hash = checksum_counts(dense);
  }

  return final_hash;
}