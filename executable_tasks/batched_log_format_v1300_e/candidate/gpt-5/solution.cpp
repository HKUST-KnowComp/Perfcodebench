#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

static inline void fnv1a_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

template <typename T>
static inline void append_uint_to_hash(uint64_t& h, T value) {
  char buf[20];
  char* p = buf;
  do {
    T q = static_cast<T>(value / 10);
    unsigned digit = static_cast<unsigned>(value - q * 10);
    *p++ = static_cast<char>('0' + digit);
    value = q;
  } while (value != 0);
  while (p != buf) {
    --p;
    fnv1a_update(h, static_cast<unsigned char>(*p));
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

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    append_uint_to_hash<uint32_t>(hash, timestamps[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    append_uint_to_hash<uint32_t>(hash, ids[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));
    append_uint_to_hash<uint16_t>(hash, levels[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));

    const std::string& msg = messages[i];
    for (unsigned char c : msg) {
      fnv1a_update(hash, c);
    }
    fnv1a_update(hash, static_cast<unsigned char>('\n'));
  }

  return hash;
}
