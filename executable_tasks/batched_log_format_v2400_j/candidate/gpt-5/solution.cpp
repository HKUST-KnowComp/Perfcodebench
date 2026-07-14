#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

inline void fnv1a_update_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

inline void fnv1a_update_bytes(uint64_t& h, const char* s, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    fnv1a_update_byte(h, static_cast<unsigned char>(s[i]));
  }
}

inline void append_uint32_hash(uint64_t& h, uint32_t v) {
  if (v == 0) {
    fnv1a_update_byte(h, '0');
    return;
  }
  char buf[10];
  int len = 0;
  while (v > 0) {
    uint32_t q = v / 10;
    uint32_t r = v - q * 10;
    buf[len++] = static_cast<char>('0' + r);
    v = q;
  }
  for (int i = len - 1; i >= 0; --i) {
    fnv1a_update_byte(h, static_cast<unsigned char>(buf[i]));
  }
}

inline void append_uint16_hash(uint64_t& h, uint16_t v) {
  if (v == 0) {
    fnv1a_update_byte(h, '0');
    return;
  }
  char buf[5];
  int len = 0;
  while (v > 0) {
    uint16_t q = static_cast<uint16_t>(v / 10);
    uint16_t r = static_cast<uint16_t>(v - q * 10);
    buf[len++] = static_cast<char>('0' + r);
    v = q;
  }
  for (int i = len - 1; i >= 0; --i) {
    fnv1a_update_byte(h, static_cast<unsigned char>(buf[i]));
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  const std::size_t n = ids.size();

  for (std::size_t i = 0; i < n; ++i) {
    append_uint32_hash(hash, timestamps[i]);
    fnv1a_update_byte(hash, '|');

    append_uint32_hash(hash, ids[i]);
    fnv1a_update_byte(hash, '|');

    append_uint16_hash(hash, levels[i]);
    fnv1a_update_byte(hash, '|');

    const std::string& msg = messages[i];
    if (!msg.empty()) {
      fnv1a_update_bytes(hash, msg.data(), msg.size());
    }
    fnv1a_update_byte(hash, '\n');
  }

  return hash;
}
