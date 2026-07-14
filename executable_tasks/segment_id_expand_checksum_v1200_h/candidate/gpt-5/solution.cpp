#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

// Fast multiply by FNV_PRIME repeated `count` times (only valid when XOR value is 0)
static inline uint64_t mul_prime_pow(uint64_t h, uint32_t count) {
  uint64_t base = FNV_PRIME;
  while (count) {
    if (count & 1u) h *= base;
    base *= base;
    count >>= 1;
  }
  return h;
}

// Repeat FNV-1a mix with constant value `v` for `count` times, unrolled by 8
static inline uint64_t mix_repeat(uint64_t h, uint64_t v, uint32_t count) {
  if (count == 0) return h;

  // Fast path for v == 0 (XOR does nothing): h *= FNV_PRIME^count
  if (v == 0) {
    return mul_prime_pow(h, count);
  }

  uint32_t r = count & 7u;
  switch (r) {
    case 7: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 6: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 5: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 4: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 3: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 2: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    case 1: h = (h ^ v) * FNV_PRIME; [[fallthrough]];
    default: break;
  }

  uint32_t n = count >> 3; // number of 8-step blocks
  while (n--) {
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
    h = (h ^ v) * FNV_PRIME;
  }
  return h;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0ULL;

  const std::size_t n = offsets.size();
  uint64_t hash = FNV_OFFSET;

  if (n >= 2) {
    const uint32_t* ptr = offsets.data();
    for (std::size_t s = 0; s + 1 < n; ++s) {
      uint32_t len = ptr[s + 1] - ptr[s];
      hash = mix_repeat(hash, static_cast<uint64_t>(s), len);
    }
  }

  return hash;
}
