#include "interface.h"

#include <cstdint>
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

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t bins = 1u << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;

  int char_map[256] = {0};
  char_map['A'] = 0;
  char_map['C'] = 1;
  char_map['G'] = 2;
  char_map['T'] = 3;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    if (dna.size() < static_cast<std::size_t>(k)) {
      hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0;
    for (int j = 0; j < k; ++j) {
      code = (code << 2) | char_map[static_cast<unsigned char>(dna[j])];
    }
    dense[code]++;

    uint32_t mask = (1u << (2 * (k - 1))) - 1;
    for (std::size_t i = k; i < dna.size(); ++i) {
      code = ((code & mask) << 2) | char_map[static_cast<unsigned char>(dna[i])];
      dense[code]++;
    }

    hash = checksum_counts(dense);
  }

  return hash;
}