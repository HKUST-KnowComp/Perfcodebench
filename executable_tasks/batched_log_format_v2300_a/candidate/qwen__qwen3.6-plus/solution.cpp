#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_hash(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
}

inline void format_and_hash_uint32(uint64_t& hash, uint32_t v) {
    if (v == 0) {
        update_hash(hash, '0');
        return;
    }
    char buf[12];
    char* p = buf + 11;
    while (v > 0) {
        *--p = '0' + (v % 10);
        v /= 10;
    }
    for (char* q = p; q < buf + 11; ++q) {
        update_hash(hash, *q);
    }
}

inline void format_and_hash_uint16(uint64_t& hash, uint16_t v) {
    if (v == 0) {
        update_hash(hash, '0');
        return;
    }
    char buf[7];
    char* p = buf + 6;
    while (v > 0) {
        *--p = '0' + (v % 10);
        v /= 10;
    }
    for (char* q = p; q < buf + 6; ++q) {
        update_hash(hash, *q);
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = FNV_OFFSET;
    for (std::size_t i = 0; i < ids.size(); ++i) {
      format_and_hash_uint32(current_hash, timestamps[i]);
      update_hash(current_hash, '|');
      format_and_hash_uint32(current_hash, ids[i]);
      update_hash(current_hash, '|');
      format_and_hash_uint16(current_hash, levels[i]);
      update_hash(current_hash, '|');
      for (char c : messages[i]) {
        update_hash(current_hash, c);
      }
      update_hash(current_hash, '\n');
    }
    hash = current_hash;
  }
  return hash;
}
