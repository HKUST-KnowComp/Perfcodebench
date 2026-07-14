#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

// Precomputed lookup: base char -> 2-bit code (0=A, 1=C, 2=G, 3=T).
// All other characters map to 0 (A).
constexpr unsigned char base_code[256] = []() {
  unsigned char arr[256] = {};
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
  const std::size_t bins = 1ULL << (2 * k);
  std::vector<uint32_t> dense(bins, 0u);
  uint64_t hash = 0;
  const std::size_t n = dna.size();
  const uint32_t mask = (1u << (2 * k)) - 1;

  for (int iter = 0; iter < iters; ++iter) {
    // Zero entire histogram (memset is typically faster than fill for wide zeroing)
    std::memset(dense.data(), 0, bins * sizeof(uint32_t));

    if (n >= static_cast<std::size_t>(k)) {
      // Compute first k-mer
      uint32_t code = 0;
      const char* ptr = dna.data();
      for (int j = 0; j < k; ++j) {
        code = (code << 2) | base_code[static_cast<unsigned char>(ptr[j])];
      }
      dense[code]++;

      // Slide the window over the rest of the sequence
      for (std::size_t i = static_cast<std::size_t>(k); i < n; ++i) {
        code = ((code << 2) | base_code[static_cast<unsigned char>(ptr[i])]) & mask;
        dense[code]++;
      }
    }

    // Compute checksum over the dense histogram
    hash = checksum_counts(dense);
  }

  return hash;
}
