#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

const uint8_t base_code[256] = {
  ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
};

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
  const uint32_t mask = static_cast<uint32_t>(bins - 1);
  uint64_t hash = 0;
  const std::size_t dna_len = dna.size();
  const std::size_t max_i = dna_len >= static_cast<std::size_t>(k) ? dna_len - k : 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, dense.size() * sizeof(uint32_t));
    if (max_i == 0) {
      hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | base_code[static_cast<unsigned char>(dna[i])];
    }
    dense[code] += 1u;

    for (std::size_t i = k; i < dna_len; ++i) {
      code = ((code << 2) | base_code[static_cast<unsigned char>(dna[i])]) & mask;
      dense[code] += 1u;
    }

    hash = checksum_counts(dense);
  }
  return hash;
}