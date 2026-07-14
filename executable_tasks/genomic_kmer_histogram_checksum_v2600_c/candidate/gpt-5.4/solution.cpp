#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
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

  const std::size_t n = dna.size();
  if (k <= 0) {
    std::vector<uint32_t> dense(1, static_cast<uint32_t>(n + 1));
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense);
    }
    return hash;
  }

  const unsigned shift = static_cast<unsigned>(2 * k);
  const std::size_t bins = std::size_t{1} << shift;
  std::vector<uint32_t> dense(bins, 0u);

  if (n < static_cast<std::size_t>(k)) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense);
    }
    return hash;
  }

  uint8_t lut[256] = {};
  lut[static_cast<unsigned char>('A')] = 0;
  lut[static_cast<unsigned char>('C')] = 1;
  lut[static_cast<unsigned char>('G')] = 2;
  lut[static_cast<unsigned char>('T')] = 3;

  const uint32_t mask = (shift >= 32) ? 0xFFFFFFFFu : ((uint32_t{1} << shift) - 1u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | lut[static_cast<unsigned char>(dna[static_cast<std::size_t>(i)])];
    }
    ++dense[code];

    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      code = ((code << 2) & mask) | lut[static_cast<unsigned char>(dna[i])];
      ++dense[code];
    }

    hash = checksum_counts(dense);
  }

  return hash;
}
