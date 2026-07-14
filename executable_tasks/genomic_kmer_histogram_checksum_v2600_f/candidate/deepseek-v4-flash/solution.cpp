#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

static const uint32_t encode_table[256] = {
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

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t n = dna.size();
  const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
  const uint64_t mask = (k == 0) ? 0ULL : ((1ULL << (2 * k)) - 1);

  std::vector<uint32_t> dense(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0);

    if (n < static_cast<std::size_t>(k)) {
      // No k-mer possible
      hash = checksum_counts(dense);
      continue;
    }

    uint64_t code = 0;
    // Encode first k characters
    for (std::size_t i = 0; i < static_cast<std::size_t>(k); ++i) {
      uint64_t v = encode_table[static_cast<unsigned char>(dna[i])];
      code = ((code << 2) | v) & mask;
    }
    ++dense[code];

    // Slide window
    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      uint64_t v = encode_table[static_cast<unsigned char>(dna[i])];
      code = ((code << 2) | v) & mask;
      ++dense[code];
    }

    hash = checksum_counts(dense);
  }

  return hash;
}
