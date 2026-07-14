#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int /*iters*/) {
  const std::size_t n = keys.size();
  if (n == 0) {
    return 0;
  }

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();
  const uint32_t th = threshold;

  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n8 = n & static_cast<std::size_t>(~7ULL);

  for (; i < n8; i += 8) {
    // Unrolled processing for 8 consecutive elements
    if (f[i]) {
      uint32_t key = k[i];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i]);
    }
    if (f[i + 1]) {
      uint32_t key = k[i + 1];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 1]);
    }
    if (f[i + 2]) {
      uint32_t key = k[i + 2];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 2]);
    }
    if (f[i + 3]) {
      uint32_t key = k[i + 3];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 3]);
    }
    if (f[i + 4]) {
      uint32_t key = k[i + 4];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 4]);
    }
    if (f[i + 5]) {
      uint32_t key = k[i + 5];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 5]);
    }
    if (f[i + 6]) {
      uint32_t key = k[i + 6];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 6]);
    }
    if (f[i + 7]) {
      uint32_t key = k[i + 7];
      if (((key & 7u) == 0u) && (key < th)) sum += static_cast<uint64_t>(p[i + 7]);
    }
  }

  for (; i < n; ++i) {
    if (f[i]) {
      uint32_t key = k[i];
      if (((key & 7u) == 0u) && (key < th)) {
        sum += static_cast<uint64_t>(p[i]);
      }
    }
  }

  return sum;
}
