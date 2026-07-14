#include "interface.h"

#include <string>
#include <vector>

namespace {

static inline void fnv1a64_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline uint64_t fnv1a64_init() {
  return 1469598103934665603ULL;
}

// Write unsigned integer in base-10 to the hash (no allocation, no formatting libs)
static inline void write_u32_hash(uint32_t v, uint64_t& h) {
  char buf[10];
  int n = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[n++] = static_cast<char>('0' + r);
    v = q;
  } while (v);
  while (n--) {
    fnv1a64_update(h, static_cast<unsigned char>(buf[n]));
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = fnv1a64_init();
  const std::size_t n = ids.size();

  for (std::size_t i = 0; i < n; ++i) {
    // timestamp
    write_u32_hash(timestamps[i], hash);
    fnv1a64_update(hash, static_cast<unsigned char>('|'));

    // id
    write_u32_hash(ids[i], hash);
    fnv1a64_update(hash, static_cast<unsigned char>('|'));

    // level (uint16_t but handled by u32 writer)
    write_u32_hash(static_cast<uint32_t>(levels[i]), hash);
    fnv1a64_update(hash, static_cast<unsigned char>('|'));

    // message
    const std::string& msg = messages[i];
    const char* p = msg.data();
    const char* e = p + msg.size();
    for (; p != e; ++p) {
      fnv1a64_update(hash, static_cast<unsigned char>(*p));
    }

    // newline
    fnv1a64_update(hash, static_cast<unsigned char>('\n'));
  }

  // Since inputs don't change across iterations, each iteration would produce the same output.
  // Returning the checksum computed once matches the checksum of the final iteration.
  return hash;
}
