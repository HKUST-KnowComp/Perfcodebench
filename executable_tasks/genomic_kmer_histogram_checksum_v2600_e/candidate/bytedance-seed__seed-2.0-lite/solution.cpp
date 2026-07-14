#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {
static const uint8_t base_code[256] = {['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3};

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
  const std::size_t dna_len = dna.size();
  const std::size_t k_size = static_cast<std::size_t>(k);
  const std::size_t k_mer_count = (dna_len >= k_size) ? (dna_len - k_size + 1) : 0;
  const std::size_t bins = 1ULL << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);

  std::vector<uint32_t> kmer_codes;
  kmer_codes.reserve(k_mer_count);

  if (k_mer_count > 0) {
    uint32_t current_code = 0;
    for (int i = 0; i < k; ++i) {
      current_code = (current_code << 2) | base_code[static_cast<unsigned char>(dna[i])];
    }
    kmer_codes.push_back(current_code);

    for (std::size_t i = 1; i < k_mer_count; ++i) {
      const char next_base = dna[i + k_size - 1];
      current_code = ((current_code << 2) | base_code[static_cast<unsigned char>(next_base)]) & mask;
      kmer_codes.push_back(current_code);
    }
  }

  std::vector<uint32_t> dense(bins, 0u);
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    for (uint32_t code : kmer_codes) {
      dense[code]++;
    }
    final_hash = checksum_counts(dense);
  }
  return final_hash;
}