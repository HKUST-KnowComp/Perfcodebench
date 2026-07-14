#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  // Determine the valid length across all input arrays to avoid out-of-bounds.
  std::size_t n = keys.size();
  if (values.size() < n) n = values.size();
  if (flags.size() < n) n = flags.size();

  // Quick early-out if there is nothing to process or the predicate cannot be satisfied.
  if (n == 0 || key_threshold == 0u) {
    return 0ull;
  }

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const uint8_t* fptr = flags.data();

  uint64_t sum = 0ull;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0ull;

    std::size_t i = 0;
    const std::size_t UNROLL = 8;
    std::size_t limit = n & ~(UNROLL - 1);

    // Unrolled main loop
    for (; i < limit; i += UNROLL) {
      {
        uint32_t k = kptr[i + 0];
        uint32_t v = vptr[i + 0];
        uint32_t c = (fptr[i + 0] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 1];
        uint32_t v = vptr[i + 1];
        uint32_t c = (fptr[i + 1] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 2];
        uint32_t v = vptr[i + 2];
        uint32_t c = (fptr[i + 2] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 3];
        uint32_t v = vptr[i + 3];
        uint32_t c = (fptr[i + 3] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 4];
        uint32_t v = vptr[i + 4];
        uint32_t c = (fptr[i + 4] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 5];
        uint32_t v = vptr[i + 5];
        uint32_t c = (fptr[i + 5] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 6];
        uint32_t v = vptr[i + 6];
        uint32_t c = (fptr[i + 6] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
      {
        uint32_t k = kptr[i + 7];
        uint32_t v = vptr[i + 7];
        uint32_t c = (fptr[i + 7] != 0);
        c &= (k < key_threshold);
        c &= ((k & 3u) == 0u);
        s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
      }
    }

    // Remainder loop
    for (; i < n; ++i) {
      uint32_t k = kptr[i];
      uint32_t v = vptr[i];
      uint32_t c = (fptr[i] != 0);
      c &= (k < key_threshold);
      c &= ((k & 3u) == 0u);
      s += static_cast<uint64_t>(v) * static_cast<uint64_t>(c);
    }

    // Match baseline behavior: return the sum from the final iteration.
    sum = s;
  }

  return sum;
}
