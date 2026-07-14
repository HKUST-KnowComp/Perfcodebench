#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL;
  }
  return hash;
}

static const uint8_t base_code[256] = []() consteval {
  uint8_t arr[256] = {0};
  arr['C'] = 1;
  arr['G'] = 2;
  arr['T'] = 3;
  return arr;
}();

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  const std::size_t bins = 1u << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);
  const uint32_t mask = static_cast<uint32_t>(bins - 1);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));
    const std::size_t dna_len = dna.size();
    if (dna_len < static_cast<std::size_t>(k)) {
      hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
      code = (code << 2) | base_code[static_cast<unsigned char>(dna[i])];
    }
    dense[code] += 1u;

    for (std::size_t i = k; i < dna_len; ++i) {
      code = (code << 2) | base_code[static_cast<unsigned char>(dna[i])];
      code &= mask;
      dense[code] += 1u;
    }

    hash = checksum_counts(dense);
  }
  return hash;
}