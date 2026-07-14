#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint32_t encode_base(char c) {
  // Matches baseline semantics: anything other than C/G/T maps to A(0).
  return (c == 'C') ? 1u : (c == 'G') ? 2u : (c == 'T') ? 3u : 0u;
}

static inline uint64_t checksum_counts(const uint32_t* counts, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
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
  const std::size_t bins = std::size_t{1} << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);

  uint64_t hash = 0;

  if (k == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense.data(), dense.size());
    }
    return hash;
  }

  if (n < static_cast<std::size_t>(k)) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense.data(), dense.size());
    }
    return hash;
  }

  const uint32_t mask = (k >= 16) ? 0xFFFFFFFFu : ((uint32_t{1} << (2 * k)) - 1u);

  std::vector<uint32_t> enc(n);
  for (std::size_t i = 0; i < n; ++i) {
    enc[i] = encode_base(dna[i]);
  }

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | enc[static_cast<std::size_t>(i)];
    }
    ++dense[code];

    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      code = ((code << 2) & mask) | enc[i];
      ++dense[code];
    }

    hash = checksum_counts(dense.data(), dense.size());
  }

  return hash;
}
