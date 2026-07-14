#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

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

  const std::size_t kk = static_cast<std::size_t>(k);
  const std::size_t n = dna.size();
  const std::size_t bins = std::size_t{1} << (2 * k);

  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;

  if (k <= 0) {
    for (int iter = 0; iter < iters; ++iter) {
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense.data(), bins);
      dense[0] = 0u;
    }
    return hash;
  }

  if (n < kk) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense.data(), bins);
    }
    return hash;
  }

  static const uint8_t enc[256] = {
      ['A'] = 0u, ['C'] = 1u, ['G'] = 2u, ['T'] = 3u,
      ['a'] = 0u, ['c'] = 1u, ['g'] = 2u, ['t'] = 3u,
  };

  const uint32_t mask = (k >= 16) ? 0xFFFFFFFFu : ((uint32_t{1} << (2 * k)) - 1u);
  const char* s = dna.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    uint32_t code = 0u;
    for (std::size_t i = 0; i < kk; ++i) {
      code = (code << 2) | static_cast<uint32_t>(enc[static_cast<unsigned char>(s[i])]);
    }
    ++dense[code];

    for (std::size_t i = kk; i < n; ++i) {
      code = ((code << 2) & mask) | static_cast<uint32_t>(enc[static_cast<unsigned char>(s[i])]);
      ++dense[code];
    }

    hash = checksum_counts(dense.data(), bins);
  }

  return hash;
}
