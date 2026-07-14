#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < bins; ++i) {
    hash ^= static_cast<uint64_t>(counts[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t n = dna.size();
  const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);

  // Precompute 2-bit encoding for each character
  // A=0, C=1, G=2, T=3
  alignas(64) uint8_t enc[256];
  std::memset(enc, 0, sizeof(enc));
  enc['A'] = 0; enc['a'] = 0;
  enc['C'] = 1; enc['c'] = 1;
  enc['G'] = 2; enc['g'] = 2;
  enc['T'] = 3; enc['t'] = 3;

  // Encode DNA string to 2-bit values
  std::vector<uint8_t> encoded(n);
  for (std::size_t i = 0; i < n; ++i) {
    encoded[i] = enc[static_cast<unsigned char>(dna[i])];
  }

  std::vector<uint32_t> dense(bins);
  uint64_t hash = 0;

  const std::size_t kk = static_cast<std::size_t>(k);
  const uint8_t* __restrict eptr = encoded.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));

    if (n < kk) {
      hash = checksum_counts(dense.data(), bins);
      continue;
    }

    // Build initial k-mer code
    uint32_t code = 0;
    for (std::size_t i = 0; i < kk; ++i) {
      code = (code << 2) | eptr[i];
    }
    dense[code]++;

    // Rolling hash for remaining k-mers
    const std::size_t limit = n - kk;
    for (std::size_t i = 1; i <= limit; ++i) {
      code = ((code << 2) | eptr[i + kk - 1]) & mask;
      dense[code]++;
    }

    hash = checksum_counts(dense.data(), bins);
  }

  return hash;
}