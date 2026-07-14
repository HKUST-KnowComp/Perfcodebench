#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

constexpr uint8_t base_code(char c) {
    switch(c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default: return 0;
    }
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
  const std::size_t bins = 1U << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);
  std::vector<uint32_t> dense(bins, 0U);

  std::vector<uint32_t> kmer_codes;
  if (dna.size() >= static_cast<std::size_t>(k)) {
    kmer_codes.reserve(dna.size() - k + 1);
    uint32_t current_code = 0;
    for (int i = 0; i < k; ++i) {
      current_code = (current_code << 2) | base_code(dna[i]);
    }
    kmer_codes.push_back(current_code);
    for (std::size_t i = k; i < dna.size(); ++i) {
      current_code = ((current_code << 2) | base_code(dna[i])) & mask;
      kmer_codes.push_back(current_code);
    }
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0U);
    for (uint32_t code : kmer_codes) {
      dense[code]++;
    }
    final_hash = checksum_counts(dense);
  }
  return final_hash;
}