#include "interface.h"

#include <vector>

// Decode a protobuf-style varint stream and sum values. Optimized by
// fusing decode and aggregation and using pointer-based unrolled decoding.
uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    uint64_t s = 0;

    while (p < end) {
      uint32_t v = 0;
      uint8_t b;

      b = *p++;
      v = static_cast<uint32_t>(b & 0x7Fu);
      if (b & 0x80u) {
        b = *p++;
        v |= static_cast<uint32_t>(b & 0x7Fu) << 7;
        if (b & 0x80u) {
          b = *p++;
          v |= static_cast<uint32_t>(b & 0x7Fu) << 14;
          if (b & 0x80u) {
            b = *p++;
            v |= static_cast<uint32_t>(b & 0x7Fu) << 21;
            if (b & 0x80u) {
              b = *p++;
              v |= static_cast<uint32_t>(b & 0x7Fu) << 28;
              // If continuation still set (values > 32 bits), skip remaining bytes.
              // Dataset is expected to fit within 5 bytes; this is a safety fast-forward.
              if (b & 0x80u) {
                do { b = *p++; } while (b & 0x80u);
              }
            }
          }
        }
      }

      s += static_cast<uint64_t>(v);
    }

    // Match baseline semantics: return the sum from the last iteration.
    sum = s;
  }

  return sum;
}
