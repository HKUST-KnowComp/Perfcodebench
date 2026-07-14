#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint32_t code_of(char c) {
  return (c == 'A') ? 0u : ((c == 'C') ? 1u : ((c == 'G') ? 2u : 3u));
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
  const std::size_t bins = 1u << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1u);
  std::vector<uint32_t> counts(bins, 0u);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0u);
    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | code_of(dna[static_cast<std::size_t>(i)]);
    }
    counts[code] += 1u;
    for (std::size_t i = static_cast<std::size_t>(k); i < dna.size(); ++i) {
      code = ((code << 2) & mask) | code_of(dna[i]);
      counts[code] += 1u;
    }
    hash = checksum_counts(counts);
  }
  return hash;
}
