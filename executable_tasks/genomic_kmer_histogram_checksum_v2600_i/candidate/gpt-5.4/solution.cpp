#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint32_t base_code(char c) {
  // Baseline semantics: anything other than C/G/T maps to 0 (same as A).
  if (c == 'C') return 1u;
  if (c == 'G') return 2u;
  if (c == 'T') return 3u;
  return 0u;
}

static inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  if (iters <= 0) return 0;
  if (k < 0) return 0;

  const std::size_t n = dna.size();
  const unsigned shift = static_cast<unsigned>(2 * k);
  const std::size_t bins = std::size_t{1} << shift;
  std::vector<uint32_t> dense(bins, 0u);

  uint64_t hash = 0;

  if (k == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense.data(), bins);
      dense[0] = 0u;
    }
    return hash;
  }

  if (n < static_cast<std::size_t>(k)) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense.data(), bins);
    }
    return hash;
  }

  const uint32_t mask = (shift >= 32) ? 0xFFFFFFFFu : ((uint32_t{1} << shift) - 1u);
  const char* s = dna.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | base_code(s[i]);
    }
    ++dense[code];

    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      code = ((code << 2) & mask) | base_code(s[i]);
      ++dense[code];
    }

    hash = checksum_counts(dense.data(), bins);
  }

  return hash;
}
