#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL; // FNV-1a prime
  }
  return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  // Match baseline behavior: if iters <= 0, return initial hash value (0) without work
  if (iters <= 0) return 0ULL;

  // Guard trivial cases
  if (k <= 0) {
    // bins = 1 << (2*k) would be 1 when k==0; produce checksum of a single zero bin
    std::vector<uint32_t> dense(1u, 0u);
    return checksum_counts(dense);
  }

  const std::size_t n = dna.size();
  if (n < static_cast<std::size_t>(k)) {
    // No k-mers present; histogram is all zeros of size 1 << (2*k)
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    return checksum_counts(dense);
  }

  const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);

  // Build simple lookup table: default 0, 'C'=1, 'G'=2, 'T'=3 (matches baseline encode)
  uint8_t lut[256] = {};
  lut[static_cast<unsigned char>('C')] = 1u;
  lut[static_cast<unsigned char>('G')] = 2u;
  lut[static_cast<unsigned char>('T')] = 3u;

  const unsigned char* s = reinterpret_cast<const unsigned char*>(dna.data());
  const std::size_t mask = bins - 1u;

  uint32_t code = 0u;
  // Initialize with first k characters
  for (int i = 0; i < k; ++i) {
    code = (code << 2) | lut[s[i]];
  }
  dense[code] += 1u;

  // Slide over the remaining characters
  for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
    code = static_cast<uint32_t>(((static_cast<std::size_t>(code) << 2) & mask) | lut[s[i]]);
    dense[code] += 1u;
  }

  return checksum_counts(dense);
}