#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;

  const size_t n = keys.size();
  const uint32_t* keys_ptr = keys.data();
  const uint8_t* flags_ptr = flags.data();
  const uint32_t* payloads_ptr = payloads.data();

  std::vector<size_t> indices;
  indices.reserve(n);
  for (size_t i = 0; i < n; ++i) {
    if (flags_ptr[i] && keys_ptr[i] < threshold && ((keys_ptr[i] & 7u) == 0u)) {
      indices.push_back(i);
    }
  }

  uint64_t sum = 0;
  for (size_t idx : indices) {
    sum += payloads_ptr[idx];
  }
  return sum;
}
