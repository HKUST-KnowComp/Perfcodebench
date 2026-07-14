#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

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
  const std::size_t bins = std::size_t(1) << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;

  uint8_t cmap[256] = {0};
  cmap['C'] = 1; cmap['G'] = 2; cmap['T'] = 3;
  cmap['c'] = 1; cmap['g'] = 2; cmap['t'] = 3;

  const uint32_t mask = (1u << (2 * k)) - 1;
  const char* data = dna.data();
  const std::size_t len = dna.size();
  const std::size_t k_sz = static_cast<std::size_t>(k);

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));

    if (len < k_sz) {
      hash = checksum_counts(dense);
      continue;
    }

    uint32_t code = 0;
    for (std::size_t i = 0; i < k_sz; ++i) {
      code = ((code << 2) & mask) | cmap[static_cast<unsigned char>(data[i])];
    }
    dense[code]++;

    for (std::size_t i = k_sz; i < len; ++i) {
      code = ((code << 2) & mask) | cmap[static_cast<unsigned char>(data[i])];
      dense[code]++;
    }

    hash = checksum_counts(dense);
  }
  return hash;
}