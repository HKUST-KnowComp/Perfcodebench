#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = counts.size();
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

  const std::size_t kk = static_cast<std::size_t>(k);
  const std::size_t n = dna.size();

  if (k >= 16) {
    return 0;
  }

  const std::size_t bins = std::size_t{1} << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);

  uint8_t lut[256] = {};
  lut[static_cast<unsigned char>('A')] = 0u;
  lut[static_cast<unsigned char>('C')] = 1u;
  lut[static_cast<unsigned char>('G')] = 2u;
  lut[static_cast<unsigned char>('T')] = 3u;

  uint64_t hash = 0;

  if (kk == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense);
    }
    return hash;
  }

  if (n < kk) {
    for (int iter = 0; iter < iters; ++iter) {
      for (std::size_t i = 0; i < bins; ++i) dense[i] = 0u;
      hash = checksum_counts(dense);
    }
    return hash;
  }

  const uint32_t mask = static_cast<uint32_t>(bins - 1);

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < bins; ++i) dense[i] = 0u;

    uint32_t code = 0u;
    for (std::size_t i = 0; i < kk; ++i) {
      code = (code << 2) | static_cast<uint32_t>(lut[static_cast<unsigned char>(dna[i])]);
    }
    ++dense[code];

    for (std::size_t i = kk; i < n; ++i) {
      code = ((code << 2) & mask) | static_cast<uint32_t>(lut[static_cast<unsigned char>(dna[i])]);
      ++dense[code];
    }

    hash = checksum_counts(dense);
  }

  return hash;
}
