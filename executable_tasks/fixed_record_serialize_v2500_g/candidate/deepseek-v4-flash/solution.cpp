#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int /*iters*/) {
  const std::size_t n = ids.size();
  if (n == 0) {
    return 1469598103934665603ULL;
  }

  const std::size_t total_bytes = n * 12;
  std::vector<uint8_t> buf(total_bytes);
  uint8_t* ptr = buf.data();

  uint64_t hash = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v;

    v = ids[i];
    *ptr = static_cast<uint8_t>(v);       hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 8);  hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 16); hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 24); hash ^= *ptr; hash *= fnv_prime; ++ptr;

    v = ts[i];
    *ptr = static_cast<uint8_t>(v);       hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 8);  hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 16); hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(v >> 24); hash ^= *ptr; hash *= fnv_prime; ++ptr;

    uint16_t w = levels[i];
    *ptr = static_cast<uint8_t>(w);       hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(w >> 8);  hash ^= *ptr; hash *= fnv_prime; ++ptr;

    w = lens[i];
    *ptr = static_cast<uint8_t>(w);       hash ^= *ptr; hash *= fnv_prime; ++ptr;
    *ptr = static_cast<uint8_t>(w >> 8);  hash ^= *ptr; hash *= fnv_prime; ++ptr;
  }

  return hash;
}
