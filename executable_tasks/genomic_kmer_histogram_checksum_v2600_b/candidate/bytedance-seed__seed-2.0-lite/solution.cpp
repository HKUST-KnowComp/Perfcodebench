#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Lookup table to convert ASCII DNA bases to 2-bit codes, matches original encode logic
static const uint8_t base_to_code[256] = []{
    uint8_t arr[256] = {0};
    arr['A'] = 0;
    arr['C'] = 1;
    arr['G'] = 2;
    arr['T'] = 3;
    return arr;
}();

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
  const uint32_t mask = bins - 1;
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    const std::size_t n = dna.size();
    if (n < static_cast<std::size_t>(k)) {
      hash = checksum_counts(dense);
      continue;
    }

    // Compute initial k-mer code
    uint32_t code = 0;
    for (std::size_t i = 0; i < static_cast<std::size_t>(k); ++i) {
      code = (code << 2) | base_to_code[static_cast<unsigned char>(dna[i])];
    }
    dense[code] += 1;

    // Roll forward through remaining bases to compute all other k-mer codes
    for (std::size_t i = k; i < n; ++i) {
      code = ((code << 2) | base_to_code[static_cast<unsigned char>(dna[i])]) & mask;
      dense[code] += 1;
    }

    hash = checksum_counts(dense);
  }
  return hash;
}