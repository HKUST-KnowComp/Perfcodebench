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

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const size_t dna_len = dna.size();
  if (dna_len < static_cast<size_t>(k)) {
    const size_t bins = 1ULL << (2*k);
    std::vector<uint32_t> dense(bins, 0);
    return checksum_counts(dense);
  }

  const uint32_t mask = (1U << (2*k)) - 1;
  const size_t bins = static_cast<size_t>(1ULL << (2*k));
  std::vector<uint32_t> dense(bins, 0U);
  uint64_t final_hash = 0;

  static constexpr uint8_t base_lut[256] = {
    ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
  };

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0U);
    uint32_t current_code = 0;
    for (int i = 0; i < k; ++i) {
      current_code = (current_code << 2) | base_lut[static_cast<unsigned char>(dna[i])];
    }
    dense[current_code]++;

    const size_t num_kmers = dna_len - k + 1;
    for (size_t i = 1; i < num_kmers; ++i) {
      current_code = ((current_code << 2) | base_lut[static_cast<unsigned char>(dna[i + k - 1])]) & mask;
      dense[current_code]++;
    }
    final_hash = checksum_counts(dense);
  }
  return final_hash;
}