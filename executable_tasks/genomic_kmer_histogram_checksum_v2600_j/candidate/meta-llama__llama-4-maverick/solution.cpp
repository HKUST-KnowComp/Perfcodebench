#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

uint32_t encode_kmer(const char* s, int k) {
  uint32_t code = 0u;
  for (int i = 0; i < k; ++i) {
    uint32_t v = 0u;
    char c = s[i];
    if (c == 'C') v = 1u;
    else if (c == 'G') v = 2u;
    else if (c == 'T') v = 3u;
    code = (code << 2) | v;
  }
  return code;
}

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
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(dense.begin(), dense.end(), 0u);
    for (std::size_t i = 0; i + static_cast<std::size_t>(k) <= dna.size(); ++i) {
      dense[encode_kmer(dna.data() + i, k)] += 1u;
    }
    hash = checksum_counts(dense);
  }
  return hash;
}
