#include "interface.h"

#include <cstdint>
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

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  if (iters <= 0) {
    return 0;
  }
  const std::size_t bins = 1ULL << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);

  const std::size_t n = dna.size();
  const std::size_t k_stride = static_cast<std::size_t>(k);
  if (k_stride <= n) {
    // Character to 2-bit code lookup (default 'A' and others -> 0)
    unsigned char_to_val[256] = {};
    char_to_val[static_cast<unsigned char>('C')] = 1;
    char_to_val[static_cast<unsigned char>('G')] = 2;
    char_to_val[static_cast<unsigned char>('T')] = 3;

    const uint64_t mask = (1ULL << (2 * k)) - 1ULL;
    uint64_t code = 0;

    // Process first k-mer
    for (std::size_t i = 0; i < k_stride; ++i) {
      code = (code << 2) | char_to_val[static_cast<unsigned char>(dna[i])];
    }
    dense[static_cast<std::size_t>(code)]++;

    // Slide window over remaining positions
    for (std::size_t i = k_stride; i < n; ++i) {
      code = ((code << 2) & mask) | char_to_val[static_cast<unsigned char>(dna[i])];
      dense[static_cast<std::size_t>(code)]++;
    }
  }

  return checksum_counts(dense);
}
