#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void fnv1a_update_char(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= FNV_PRIME;
}

inline void fnv1a_update(uint64_t& h, const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < len; ++i) {
    h ^= static_cast<uint64_t>(p[i]);
    h *= FNV_PRIME;
  }
}

inline void hash_uint32_decimal(uint64_t& h, uint32_t v) {
  if (v == 0) {
    fnv1a_update_char(h, '0');
    return;
  }
  char buf[10];
  int idx = 0;
  while (v > 0) {
    uint32_t q = v / 10;
    uint32_t r = v - q * 10; // equivalent to v % 10 but can be faster
    buf[idx++] = static_cast<char>('0' + r);
    v = q;
  }
  while (idx--) {
    fnv1a_update_char(h, static_cast<unsigned char>(buf[idx]));
  }
}

inline void hash_uint16_decimal(uint64_t& h, uint16_t v) {
  if (v == 0) {
    fnv1a_update_char(h, '0');
    return;
  }
  char buf[5];
  int idx = 0;
  uint32_t val = v;
  while (val > 0) {
    uint32_t q = val / 10;
    uint32_t r = val - q * 10;
    buf[idx++] = static_cast<char>('0' + r);
    val = q;
  }
  while (idx--) {
    fnv1a_update_char(h, static_cast<unsigned char>(buf[idx]));
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
    return 0ULL;
  }

  uint64_t hash = FNV_OFFSET;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    // timestamp
    hash_uint32_decimal(hash, timestamps[i]);
    fnv1a_update_char(hash, '|');
    // id
    hash_uint32_decimal(hash, ids[i]);
    fnv1a_update_char(hash, '|');
    // level
    hash_uint16_decimal(hash, levels[i]);
    fnv1a_update_char(hash, '|');
    // message
    const std::string& msg = messages[i];
    if (!msg.empty()) {
      fnv1a_update(hash, msg.data(), msg.size());
    }
    // newline
    fnv1a_update_char(hash, '\n');
  }

  return hash;
}
