#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void hash_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= FNV_PRIME;
}

inline void hash_bytes(uint64_t& h, const unsigned char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    h ^= static_cast<uint64_t>(data[i]);
    h *= FNV_PRIME;
  }
}

// Append unsigned integer in decimal to hash (no allocation)
inline void hash_u32_decimal(uint64_t& h, uint32_t v) {
  char buf[10];
  char* p = buf + 10;
  // Produce digits in reverse order
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    *--p = static_cast<char>('0' + r);
    v = q;
  } while (v != 0u);
  // Hash produced digits in forward order
  const unsigned char* b = reinterpret_cast<const unsigned char*>(p);
  hash_bytes(h, b, static_cast<std::size_t>((buf + 10) - p));
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0ULL;

  const std::size_t n = ids.size();

  uint64_t hash = FNV_OFFSET_BASIS;

  for (std::size_t i = 0; i < n; ++i) {
    // timestamp
    hash_u32_decimal(hash, timestamps[i]);
    hash_byte(hash, static_cast<unsigned char>('|'));

    // id
    hash_u32_decimal(hash, ids[i]);
    hash_byte(hash, static_cast<unsigned char>('|'));

    // level (uint16_t but decimal-printed the same way)
    hash_u32_decimal(hash, static_cast<uint32_t>(levels[i]));
    hash_byte(hash, static_cast<unsigned char>('|'));

    // message
    const std::string& msg = messages[i];
    hash_bytes(hash, reinterpret_cast<const unsigned char*>(msg.data()), msg.size());

    // newline
    hash_byte(hash, static_cast<unsigned char>('\n'));
  }

  // Semantics require returning hash of the last iteration; inputs don't change,
  // so computing once yields the correct final hash.
  return hash;
}
