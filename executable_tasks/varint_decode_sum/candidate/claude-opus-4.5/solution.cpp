#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (encoded.empty() || iters <= 0) {
    return 0;
  }

  const uint8_t* const data = encoded.data();
  const std::size_t size = encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* ptr = data;
    const uint8_t* const end = data + size;

    // Process bytes
    while (ptr < end) {
      // Fast path: single-byte varint (most common case, value < 128)
      uint8_t byte = *ptr;
      if ((byte & 0x80) == 0) {
        local_sum += byte;
        ++ptr;
        continue;
      }

      // Multi-byte varint decoding
      uint64_t value = byte & 0x7F;
      ++ptr;
      
      if (ptr < end) {
        byte = *ptr;
        value |= static_cast<uint64_t>(byte & 0x7F) << 7;
        ++ptr;
        if ((byte & 0x80) != 0 && ptr < end) {
          byte = *ptr;
          value |= static_cast<uint64_t>(byte & 0x7F) << 14;
          ++ptr;
          if ((byte & 0x80) != 0 && ptr < end) {
            byte = *ptr;
            value |= static_cast<uint64_t>(byte & 0x7F) << 21;
            ++ptr;
            if ((byte & 0x80) != 0 && ptr < end) {
              byte = *ptr;
              value |= static_cast<uint64_t>(byte & 0x7F) << 28;
              ++ptr;
              // Handle up to 10 bytes for uint64_t
              if ((byte & 0x80) != 0 && ptr < end) {
                byte = *ptr;
                value |= static_cast<uint64_t>(byte & 0x7F) << 35;
                ++ptr;
                if ((byte & 0x80) != 0 && ptr < end) {
                  byte = *ptr;
                  value |= static_cast<uint64_t>(byte & 0x7F) << 42;
                  ++ptr;
                  if ((byte & 0x80) != 0 && ptr < end) {
                    byte = *ptr;
                    value |= static_cast<uint64_t>(byte & 0x7F) << 49;
                    ++ptr;
                    if ((byte & 0x80) != 0 && ptr < end) {
                      byte = *ptr;
                      value |= static_cast<uint64_t>(byte & 0x7F) << 56;
                      ++ptr;
                      if ((byte & 0x80) != 0 && ptr < end) {
                        byte = *ptr;
                        value |= static_cast<uint64_t>(byte & 0x7F) << 63;
                        ++ptr;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      local_sum += value;
    }
    sum = local_sum;
  }

  return sum;
}