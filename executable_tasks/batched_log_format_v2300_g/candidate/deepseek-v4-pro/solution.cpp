#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

void update_hash_uint32(uint32_t value, uint64_t& hash) {
    if (value == 0) {
        hash ^= static_cast<uint8_t>('0');
        hash *= 1099511628211ULL;
        return;
    }
    char buf[10];
    int pos = 0;
    while (value > 0) {
        buf[pos++] = '0' + (value % 10);
        value /= 10;
    }
    for (int i = pos - 1; i >= 0; --i) {
        hash ^= static_cast<uint8_t>(buf[i]);
        hash *= 1099511628211ULL;
    }
}

void update_hash_uint16(uint16_t value, uint64_t& hash) {
    if (value == 0) {
        hash ^= static_cast<uint8_t>('0');
        hash *= 1099511628211ULL;
        return;
    }
    char buf[5];
    int pos = 0;
    while (value > 0) {
        buf[pos++] = '0' + (value % 10);
        value /= 10;
    }
    for (int i = pos - 1; i >= 0; --i) {
        hash ^= static_cast<uint8_t>(buf[i]);
        hash *= 1099511628211ULL;
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    update_hash_uint32(timestamps[i], hash);
    hash ^= static_cast<uint8_t>('|');
    hash *= 1099511628211ULL;

    update_hash_uint32(ids[i], hash);
    hash ^= static_cast<uint8_t>('|');
    hash *= 1099511628211ULL;

    update_hash_uint16(levels[i], hash);
    hash ^= static_cast<uint8_t>('|');
    hash *= 1099511628211ULL;

    for (char c : messages[i]) {
      hash ^= static_cast<uint8_t>(c);
      hash *= 1099511628211ULL;
    }

    hash ^= static_cast<uint8_t>('\n');
    hash *= 1099511628211ULL;
  }

  return hash;
}