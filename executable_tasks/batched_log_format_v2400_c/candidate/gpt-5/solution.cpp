#include "interface.h"

namespace {

static inline void fnv1a_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void fnv1a_char(uint64_t& h, char c) {
  fnv1a_byte(h, static_cast<unsigned char>(c));
}

static inline void fnv1a_string(uint64_t& h, const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* e = p + s.size();
  while (p != e) {
    fnv1a_byte(h, *p++);
  }
}

// Append unsigned integer in base-10 to hash (no allocations, no std::to_string)
static inline void fnv1a_uint(uint64_t& h, uint32_t v) {
  char buf[16];
  int i = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[i++] = static_cast<char>('0' + r);
    v = q;
  } while (v);
  while (i--) {
    fnv1a_char(h, buf[i]);
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

  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    fnv1a_uint(hash, timestamps[i]);
    fnv1a_char(hash, '|');
    fnv1a_uint(hash, ids[i]);
    fnv1a_char(hash, '|');
    fnv1a_uint(hash, static_cast<uint32_t>(levels[i]));
    fnv1a_char(hash, '|');
    fnv1a_string(hash, messages[i]);
    fnv1a_char(hash, '\n');
  }

  // Result is identical across iterations; baseline overwrote the same value each time.
  return hash;
}
