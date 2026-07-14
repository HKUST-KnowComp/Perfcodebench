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
  const std::size_t n = dna.size();
  const std::size_t bins = 1u << (2 * k);
  std::vector<uint32_t> dense(bins);
  const uint32_t mask = (1u << (2 * k)) - 1;

  static const uint8_t lut[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0......
  };
  // Correct initialization for A, C, G, T
  // A=65, C=67, G=71, T=84
  // We'll just use a simple runtime init to avoid large static array literals and ensure correctness.
  uint8_t char_map[256] = {0};
  char_map['A'] = 0;
  char_map['C'] = 1;
  char_map['G'] = 2;
  char_map['T'] = 3;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    if (n >= static_cast<std::size_t>(k)) {
      uint32_t code = 0;
      for (std::size_t i = 0; i < n; ++i) {
        code = ((code << 2) | char_map[static_cast<unsigned char>(dna[i])]) & mask;
        if (i >= static_cast<std::size_t>(k) - 1) {
          dense[code]++;
        }
      }
    }
    hash = checksum_counts(dense);
  }
  return hash;
}
