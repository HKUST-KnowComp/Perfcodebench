#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

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
  static const uint8_t base_code[256] = []() consteval {
    uint8_t arr[256] = {0};
    arr['A'] = 0; arr['C'] = 1; arr['G'] = 2; arr['T'] = 3;
    return arr;
  }();

  const size_t dna_len = dna.size();
  const int k2 = 2 * k;
  const size_t bins = 1ULL << k2;
  const uint32_t mask = bins - 1;
  std::vector<uint32_t> dense(bins, 0);
  uint64_t final_hash = 0;
  const size_t max_i = dna_len >= static_cast<size_t>(k) ? (dna_len - static_cast<size_t>(k)) : 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));
    if (dna_len < static_cast<size_t>(k)) {
      final_hash = checksum_counts(dense);
      continue;
    }

    uint32_t current_code = 0;
    for (int i = 0; i < k; ++i) {
      current_code = (current_code << 2) | base_code[static_cast<unsigned char>(dna[i])];
    }
    dense[current_code]++;

    for (size_t i = 1; i <= max_i; ++i) {
      const char next = dna[i + k - 1];
      current_code = ((current_code << 2) | base_code[static_cast<unsigned char>(next)]) & mask;
      dense[current_code]++;
    }

    final_hash = checksum_counts(dense);
  }
  return final_hash;
}