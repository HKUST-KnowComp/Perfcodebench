#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

// 64-bit FNV-1a
static inline void hash_add_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void hash_add_str(uint64_t& h, const std::string& s) {
  for (unsigned char c : s) {
    hash_add_byte(h, c);
  }
}

// Append unsigned integer in decimal to hash (no allocation)
static inline void hash_add_uint_dec(uint64_t& h, uint32_t v) {
  // Max 10 digits for uint32_t
  char buf[10];
  int i = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[i++] = static_cast<char>('0' + r);
    v = q;
  } while (v != 0u);
  while (i--) {
    hash_add_byte(h, static_cast<unsigned char>(buf[i]));
  }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = ids.size();
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  for (std::size_t i = 0; i < n; ++i) {
    // timestamp
    hash_add_uint_dec(hash, timestamps[i]);
    hash_add_byte(hash, static_cast<unsigned char>('|'));
    // id
    hash_add_uint_dec(hash, ids[i]);
    hash_add_byte(hash, static_cast<unsigned char>('|'));
    // level (uint16_t promoted)
    hash_add_uint_dec(hash, static_cast<uint32_t>(levels[i]));
    hash_add_byte(hash, static_cast<unsigned char>('|'));
    // message
    hash_add_str(hash, messages[i]);
    // newline
    hash_add_byte(hash, static_cast<unsigned char>('\n'));
  }

  return hash;
}
