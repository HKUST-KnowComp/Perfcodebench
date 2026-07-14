#include "interface.h"

#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit constants
static inline void fnv1a_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void fnv1a_update_decimal_uint32(uint64_t& h, uint32_t v) {
  if (v == 0) {
    fnv1a_update(h, static_cast<unsigned char>('0'));
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
    fnv1a_update(h, static_cast<unsigned char>(buf[i]));
  }
}

static inline void fnv1a_update_decimal_uint16(uint64_t& h, uint16_t v) {
  if (v == 0) {
    fnv1a_update(h, static_cast<unsigned char>('0'));
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
    fnv1a_update(h, static_cast<unsigned char>(buf[i]));
  }
}

static inline void fnv1a_update_str(uint64_t& h, const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* e = p + s.size();
  while (p != e) {
    fnv1a_update(h, *p++);
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

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    fnv1a_update_decimal_uint32(hash, timestamps[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    fnv1a_update_decimal_uint32(hash, ids[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    fnv1a_update_decimal_uint16(hash, levels[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    fnv1a_update_str(hash, messages[i]);
    fnv1a_update(hash, static_cast<unsigned char>('\n'));
  }

  return hash;
}
