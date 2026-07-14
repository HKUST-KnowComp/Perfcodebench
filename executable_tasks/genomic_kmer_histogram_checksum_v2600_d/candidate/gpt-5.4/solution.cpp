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
  if (k < 0) return 0;

  const std::size_t n = dna.size();
  const unsigned uk = static_cast<unsigned>(k);

  std::size_t bins = 1;
  if (uk > 0) {
    bins = std::size_t{1} << (2u * uk);
  }

  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;

  alignas(64) static constexpr unsigned char lut[256] = {
      /*   0- 15 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /*  16- 31 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /*  32- 47 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /*  48- 63 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /*  64- 79 */ 0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,
      /*  80- 95 */ 0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,
      /*  96-111 */ 0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,
      /* 112-127 */ 0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,
      /* 128-143 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 144-159 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 160-175 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 176-191 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 192-207 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 208-223 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 224-239 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      /* 240-255 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

  if (k == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      dense[0] = static_cast<uint32_t>(n + 1);
      hash = checksum_counts(dense.data(), bins);
      dense[0] = 0;
    }
    return hash;
  }

  if (n < static_cast<std::size_t>(k)) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = checksum_counts(dense.data(), bins);
    }
    return hash;
  }

  const uint32_t mask = (uk >= 16) ? 0xFFFFFFFFu : ((uint32_t{1} << (2u * uk)) - 1u);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | static_cast<uint32_t>(lut[static_cast<unsigned char>(dna[static_cast<std::size_t>(i)])]);
    }
    ++dense[code];

    for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
      code = ((code << 2) & mask) | static_cast<uint32_t>(lut[static_cast<unsigned char>(dna[i])]);
      ++dense[code];
    }

    hash = checksum_counts(dense.data(), bins);
  }

  return hash;
}
