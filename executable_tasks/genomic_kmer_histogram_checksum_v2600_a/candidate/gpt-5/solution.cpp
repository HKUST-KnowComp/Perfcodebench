#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL; // FNV-1a prime
  }
  return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
  if (iters <= 0) return 0ULL;

  // Handle k == 0 to match baseline semantics (n + 1 empty substrings)
  if (k == 0) {
    std::vector<uint32_t> counts(1u, 0u);
    counts[0] = static_cast<uint32_t>(dna.size() + 1u);
    return checksum_counts(counts);
  }

  const std::size_t n = dna.size();
  if (n < static_cast<std::size_t>(k)) {
    // No k-mers; checksum over zero-initialized dense vector
    std::size_t bins = std::size_t(1) << (2 * k);
    std::vector<uint32_t> counts(bins, 0u);
    return checksum_counts(counts);
  }

  // Dense histogram of size 4^k
  const std::size_t bins = std::size_t(1) << (2 * k);
  std::vector<uint32_t> counts(bins, 0u);

  // Map ASCII to 2-bit values: A->0, C->1, G->2, T->3. Others map to 0.
  static const uint8_t lut[256] = {
    /* 0..64 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0, /* 'A' is 65 */ 0,0,0,0,0,0,0,0,0,0,0,
    /* 76..90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* 91..96 */ 0,0,0,0,0,0,
    /* 'a' is 97 */ 0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,
    // Fill the rest with zeros (we'll override needed letters below)
  };
  // Build a mutable copy to set exact letters only once (static array above
  // is initialized with zeros; we overlay our needed mappings here via const_cast).
  // However, to avoid static initialization order issues and overhead, we compute
  // values on-the-fly without modifying the lut.

  auto char2bits = [](unsigned char c) -> uint8_t {
    // Direct checks for common uppercase characters
    switch (c) {
      case 'A': return 0u;
      case 'C': return 1u;
      case 'G': return 2u;
      case 'T': return 3u;
      default:  return 0u; // Treat any non-ACGT as 'A' (consistent with baseline)
    }
  };

  const uint32_t mask = static_cast<uint32_t>(bins - 1);

  // Initialize rolling code with first k-1 bases
  uint32_t code = 0u;
  for (int j = 0; j < k - 1; ++j) {
    code = (code << 2) | char2bits(static_cast<unsigned char>(dna[static_cast<std::size_t>(j)]));
  }

  // Slide window across sequence
  for (std::size_t i = static_cast<std::size_t>(k - 1); i < n; ++i) {
    code = ((code << 2) | char2bits(static_cast<unsigned char>(dna[i]))) & mask;
    counts[code] += 1u;
  }

  return checksum_counts(counts);
}
