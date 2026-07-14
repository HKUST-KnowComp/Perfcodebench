#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Fast bitset for 16-bit codes (64 Ki bits = 8 KiB, stack-allocated)
  alignas(64) uint64_t present[1024] = {0};  // 1024 * 64 = 65536 bits

  const uint16_t* code_ptr = codes.data();
  const size_t n = codes.size();

  // Single pass: mark every index we actually use
  for (size_t i = 0; i < n; ++i) {
    const uint16_t c = code_ptr[i];
    present[c >> 6] |= uint64_t(1) << (c & 63);
  }

  // Count how many dictionary entries are referenced
  uint64_t distinct = 0;
  for (size_t w = 0; w < 1024; ++w) {
    distinct += __builtin_popcountll(present[w]);
  }

  // Iterations are free now
  return distinct;
}