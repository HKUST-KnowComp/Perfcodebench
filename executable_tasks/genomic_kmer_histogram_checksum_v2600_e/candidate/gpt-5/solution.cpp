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
  uint64_t hash = 0;

  // Map ASCII chars to 2-bit values: 'A' and all non-C/G/T -> 0, 'C'->1, 'G'->2, 'T'->3
  unsigned char mapv[256] = {};
  mapv[static_cast<unsigned char>('C')] = 1u;
  mapv[static_cast<unsigned char>('G')] = 2u;
  mapv[static_cast<unsigned char>('T')] = 3u;

  const std::size_t n = dna.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    if (k == 0) {
      // Count of empty substring at each position plus one at end
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense);
      continue;
    }

    if (static_cast<std::size_t>(k) > n) {
      // No k-mers; histogram remains zeroed
      hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | mapv[static_cast<unsigned char>(dna[static_cast<std::size_t>(i)])];
    }
    dense[code] += 1u;

    const uint32_t mask = static_cast<uint32_t>(bins - 1u);
    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      code = ((code << 2) | mapv[static_cast<unsigned char>(dna[i])]) & mask;
      dense[code] += 1u;
    }

    hash = checksum_counts(dense);
  }

  return hash;
}
