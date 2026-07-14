#include "interface.h"

#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  const std::size_t n = values.size();
  const uint8_t* fptr = flags.data();
  const uint32_t* vptr = values.data();

  uint64_t hash = FNV_OFFSET;

  // First pass: values with flag != 0 (left partition, stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (fptr[i]) {
      hash ^= vptr[i];
      hash *= FNV_PRIME;
    }
  }
  // Second pass: values with flag == 0 (right partition, stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (!fptr[i]) {
      hash ^= vptr[i];
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
