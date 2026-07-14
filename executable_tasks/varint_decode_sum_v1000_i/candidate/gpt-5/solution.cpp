#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (iters <= 0) {
    return 0ull;
  }
  if (encoded.empty()) {
    return 0ull;
  }

  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();

  uint64_t last_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    uint64_t sum = 0;

    while (p < end) {
      uint32_t v;
      uint8_t b;

      b = *p++;
      v = static_cast<uint32_t>(b & 0x7Fu);
      if ((b & 0x80u) == 0u) {
        sum += static_cast<uint64_t>(v);
        continue;
      }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 7;
      if ((b & 0x80u) == 0u) {
        sum += static_cast<uint64_t>(v);
        continue;
      }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 14;
      if ((b & 0x80u) == 0u) {
        sum += static_cast<uint64_t>(v);
        continue;
      }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 21;
      if ((b & 0x80u) == 0u) {
        sum += static_cast<uint64_t>(v);
        continue;
      }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 28;
      if ((b & 0x80u) != 0u) {
        // Skip any superfluous continuation bytes if present.
        do {
          b = *p++;
        } while ((b & 0x80u) != 0u);
      }
      sum += static_cast<uint64_t>(v);
    }

    last_sum = sum;
  }

  return last_sum;
}
