#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace {

// Lookup table: char -> 2-bit value (A=0, C=1, G=2, T=3, else 0)
uint32_t char_to_val[256] = {};
bool init_lut = false;

void ensure_lut() {
  if (!init_lut) {
    char_to_val['A'] = 0;
    char_to_val['C'] = 1;
    char_to_val['G'] = 2;
    char_to_val['T'] = 3;
    init_lut = true;
  }
}

// FNV-1a hash over vector of uint32_t counts
uint64_t fnv1a(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t c : counts) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
  }
  return hash;
}

} // anonymous namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  ensure_lut();

  const std::size_t n = dna.size();
  const std::size_t bits = static_cast<std::size_t>(2) * static_cast<std::size_t>(k);
  const std::size_t bins = static_cast<std::size_t>(1) << bits;
  uint64_t mask = (bits == 64) ? ~0ULL : ( (static_cast<uint64_t>(1) << bits) - 1);

  // Allocate dense histogram once
  std::vector<uint32_t> dense(bins, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);

    if (static_cast<std::size_t>(k) <= n) {
      // Build initial k-mer code
      uint64_t code = 0;
      for (std::size_t i = 0; i < static_cast<std::size_t>(k); ++i) {
        code = (code << 2) | char_to_val[static_cast<unsigned char>(dna[i])];
      }
      ++dense[code & mask];  // mask always covers bits, but indexing needs code < bins

      // Slide the window
      for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
        code = ((code << 2) | char_to_val[static_cast<unsigned char>(dna[i])]) & mask;
        ++dense[code];
      }
    }

    hash = fnv1a(dense);
  }

  return hash;
}
