#include "interface.h"

#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

inline void fnv1a_update_byte(uint64_t& h, unsigned char b) {
  h ^= static_cast<uint64_t>(b);
  h *= FNV_PRIME;
}

inline void fnv1a_update_bytes(uint64_t& h, const unsigned char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    h ^= static_cast<uint64_t>(data[i]);
    h *= FNV_PRIME;
  }
}

// Fast decimal conversion for unsigned integers (up to 32-bit)
inline void hash_uint_dec(uint64_t& h, uint32_t v) {
  if (v == 0) {
    fnv1a_update_byte(h, static_cast<unsigned char>('0'));
    return;
  }
  char buf[10]; // max 10 digits for uint32_t
  int pos = 10;
  while (v != 0) {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[--pos] = static_cast<char>('0' + r);
    v = q;
  }
  const unsigned char* p = reinterpret_cast<const unsigned char*>(buf + pos);
  fnv1a_update_bytes(h, p, static_cast<std::size_t>(10 - pos));
}

inline void hash_uint_dec16(uint64_t& h, uint16_t v16) {
  uint32_t v = v16;
  if (v == 0) {
    fnv1a_update_byte(h, static_cast<unsigned char>('0'));
    return;
  }
  char buf[5]; // max 5 digits for uint16_t
  int pos = 5;
  while (v != 0) {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[--pos] = static_cast<char>('0' + r);
    v = q;
  }
  const unsigned char* p = reinterpret_cast<const unsigned char*>(buf + pos);
  fnv1a_update_bytes(h, p, static_cast<std::size_t>(5 - pos));
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  uint64_t hash = 0; // match baseline behavior when iters == 0
  const std::size_t n = ids.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET;

    for (std::size_t i = 0; i < n; ++i) {
      // timestamps[i]
      hash_uint_dec(h, timestamps[i]);
      fnv1a_update_byte(h, static_cast<unsigned char>('|'));

      // ids[i]
      hash_uint_dec(h, ids[i]);
      fnv1a_update_byte(h, static_cast<unsigned char>('|'));

      // levels[i]
      hash_uint_dec16(h, levels[i]);
      fnv1a_update_byte(h, static_cast<unsigned char>('|'));

      // messages[i]
      const std::string& msg = messages[i];
      const unsigned char* mptr = reinterpret_cast<const unsigned char*>(msg.data());
      fnv1a_update_bytes(h, mptr, msg.size());

      // newline
      fnv1a_update_byte(h, static_cast<unsigned char>('\n'));
    }

    hash = h; // keep last iteration's hash, as in baseline
  }

  return hash;
}
