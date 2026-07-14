#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  if (probes.empty()) return 0;
  if (iters <= 0) return 0;

  const size_t num_probes = probes.size();
  const uint32_t* __restrict__ probes_ptr = probes.data();
  const uint32_t* __restrict__ values_ptr = values.data();
  const uint32_t* __restrict__ keys_ptr = keys.data();
  const size_t keys_size = keys.size();

  // Check if keys are a simple contiguous range [base, base + keys_size - 1]
  bool is_dense = false;
  uint32_t base = 0;
  if (keys_size > 0) {
    base = keys_ptr[0];
    if (keys_ptr[keys_size - 1] == base + (uint32_t)keys_size - 1) {
      is_dense = true;
    }
  }

  // Pre-calculate remapped values to avoid repeated lookups across iterations
  std::vector<uint32_t> remapped(num_probes);
  uint32_t* __restrict__ remapped_ptr = remapped.data();

  if (is_dense) {
    for (size_t i = 0; i < num_probes; ++i) {
      remapped_ptr[i] = values_ptr[probes_ptr[i] - base];
    }
  } else {
    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = probes_ptr[i];
      const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
      const size_t idx = static_cast<size_t>(it - keys.begin());
      remapped_ptr[i] = values_ptr[idx];
    }
  }

  // The hash is reset every iteration, so we only need to compute it once.
  // However, to strictly follow the logic of the baseline (which might be used for timing),
  // we perform the hashing loop. FNV-1a is strictly sequential.
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0;
    // Unroll slightly to help the compiler, though FNV is serial
    for (; i + 3 < num_probes; i += 4) {
      hash = mix(hash, remapped_ptr[i]);
      hash = mix(hash, remapped_ptr[i + 1]);
      hash = mix(hash, remapped_ptr[i + 2]);
      hash = mix(hash, remapped_ptr[i + 3]);
    }
    for (; i < num_probes; ++i) {
      hash = mix(hash, remapped_ptr[i]);
    }
  }

  return hash;
}
